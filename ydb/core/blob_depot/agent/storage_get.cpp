#include "agent_impl.h"
#include "blob_mapping_cache.h"
#include "blocks.h"

namespace NKikimr::NBlobDepot {

    template<>
    TBlobDepotAgent::TQuery *TBlobDepotAgent::CreateQuery<TEvBlobStorage::EvGet>(std::unique_ptr<IEventHandle> ev) {
        class TGetQuery : public TBlobStorageQuery<TEvBlobStorage::TEvGet> {
            std::unique_ptr<TEvBlobStorage::TEvGetResult> Response;
            ui32 AnswersRemain;
            std::vector<TString> ValueChainsInFlight;
            std::unordered_set<std::tuple<ui64, TString>> ValueChainsWithNodata;

            struct TResolveKeyContext : TRequestContext {
                ui32 QueryIdx;

                TResolveKeyContext(ui32 queryIdx)
                    : QueryIdx(queryIdx)
                {}
            };

        public:
            using TBlobStorageQuery::TBlobStorageQuery;

            void Initiate() override {
                if (IS_LOG_PRIORITY_ENABLED(*TlsActivationContext, NLog::PRI_TRACE, NKikimrServices::BLOB_DEPOT_EVENTS)) {
                    for (ui32 i = 0; i < Request.QuerySize; ++i) {
                        const auto& q = Request.Queries[i];
                        BDEV_QUERY(BDEV19, "TEvGet_new", (U.BlobId, q.Id), (U.Shift, q.Shift), (U.Size, q.Size),
                            (U.MustRestoreFirst, Request.MustRestoreFirst), (U.IsIndexOnly, Request.IsIndexOnly));
                    }
                }

                Response = std::make_unique<TEvBlobStorage::TEvGetResult>(NKikimrProto::OK, Request.QuerySize,
                    Agent.VirtualGroupId);
                AnswersRemain = Request.QuerySize;
                ValueChainsInFlight.resize(Request.QuerySize);

                if (Request.ReaderTabletData) {
                    auto status = Agent.BlocksManager.CheckBlockForTablet(Request.ReaderTabletData->Id, Request.ReaderTabletData->Generation, this, nullptr);
                    if (status == NKikimrProto::BLOCKED) {
                        EndWithError(status, "Fail TEvGet due to BLOCKED tablet generation");
                        return;
                    }
                }

                for (ui32 i = 0; i < Request.QuerySize; ++i) {
                    auto& query = Request.Queries[i];

                    auto& response = Response->Responses[i];
                    response.Id = query.Id;
                    response.Shift = query.Shift;
                    response.RequestedSize = query.Size;

                    TString blobId = query.Id.AsBinaryString();
                    if (const TResolvedValueChain *value = Agent.BlobMappingCache.ResolveKey(blobId, this,
                            std::make_shared<TResolveKeyContext>(i))) {
                        if (!ProcessSingleResult(i, value, std::nullopt)) {
                            return; // error occured
                        }
                    } else {
                        STLOG(PRI_DEBUG, BLOB_DEPOT_AGENT, BDA29, "resolve pending", (AgentId, Agent.LogId),
                            (QueryId, GetQueryId()), (QueryIdx, i), (BlobId, query.Id));
                    }
                }

                CheckAndFinish();
            }

            bool ProcessSingleResult(ui32 queryIdx, const TResolvedValueChain *value, const std::optional<TString>& errorReason) {
                STLOG(PRI_DEBUG, BLOB_DEPOT_AGENT, BDA27, "ProcessSingleResult", (AgentId, Agent.LogId),
                    (QueryId, GetQueryId()), (QueryIdx, queryIdx), (Value, value), (ErrorReason, errorReason));

                auto& r = Response->Responses[queryIdx];
                Y_VERIFY(r.Status == NKikimrProto::UNKNOWN);
                if (errorReason) {
                    r.Status = NKikimrProto::ERROR;
                    --AnswersRemain;
                } else if (!value || value->empty()) {
                    r.Status = NKikimrProto::NODATA;
                    --AnswersRemain;
                } else if (Request.IsIndexOnly) {
                    r.Status = NKikimrProto::OK;
                    --AnswersRemain;
                } else {
                    ValueChainsInFlight[queryIdx] = GetValueChainId(*value);
                    TReadArg arg{
                        *value,
                        Request.GetHandleClass,
                        Request.MustRestoreFirst,
                        this,
                        Request.Queries[queryIdx].Shift,
                        Request.Queries[queryIdx].Size,
                        queryIdx,
                        Request.ReaderTabletData};
                    TString error;
                    auto makeValueChain = [&] {
                        TStringStream str;
                        str << '[';
                        for (int i = 0; i < value->size(); ++i) {
                            const auto& item = value->at(i);
                            if (i != 0) {
                                str << ' ';
                            }
                            const auto blobId = LogoBlobIDFromLogoBlobID(item.GetBlobId());
                            const ui64 subrangeBegin = item.GetSubrangeBegin();
                            const ui64 subrangeEnd = item.HasSubrangeEnd() ? item.GetSubrangeEnd() : blobId.BlobSize();
                            str << blobId << '@' << item.GetGroupId() << '{' << subrangeBegin << '-' << (subrangeEnd - 1) << '}';
                        }
                        str << ']';
                        return str.Str();
                    };
                    STLOG(PRI_DEBUG, BLOB_DEPOT_AGENT, BDA34, "IssueRead", (AgentId, Agent.LogId),
                        (Offset, arg.Offset), (Size, arg.Size), (ValueChain, makeValueChain()), (Tag, arg.Tag));
                    const bool success = Agent.IssueRead(arg, error);
                    if (!success) {
                        EndWithError(NKikimrProto::ERROR, std::move(error));
                        return false;
                    }
                }
                return true;
            }

            void OnRead(ui64 tag, NKikimrProto::EReplyStatus status, TString buffer) override {
                STLOG(PRI_DEBUG, BLOB_DEPOT_AGENT, BDA35, "OnRead", (AgentId, Agent.LogId),
                    (Tag, tag), (Status, status), (Buffer.size, status == NKikimrProto::OK ? buffer.size() : 0),
                    (ErrorReason, status != NKikimrProto::OK ? buffer : ""));

                if (status == NKikimrProto::NODATA) { // we have to retry this read, this may be a race between blob movement
                    const auto& q = Request.Queries[tag];
                    if (ValueChainsWithNodata.emplace(tag, std::exchange(ValueChainsInFlight[tag], {})).second) { // real race
                        const TResolvedValueChain *value = Agent.BlobMappingCache.ResolveKey(
                            q.Id.AsBinaryString(),
                            this,
                            std::make_shared<TResolveKeyContext>(tag),
                            true);
                        Y_VERIFY(!value);
                        return;
                    } else {
                        Y_VERIFY_DEBUG_S(false, "data is lost AgentId# " << Agent.LogId << " BlobId# " << q.Id);
                        STLOG(PRI_CRIT, BLOB_DEPOT_AGENT, BDA41, "failed to Get blob -- data is lost",
                            (AgentId, Agent.LogId), (BlobId, q.Id));
                        status = NKikimrProto::ERROR;
                    }
                }

                auto& resp = Response->Responses[tag];
                Y_VERIFY(resp.Status == NKikimrProto::UNKNOWN);
                resp.Status = status;
                if (status == NKikimrProto::OK) {
                    resp.Buffer = std::move(buffer);
                }
                --AnswersRemain;
                CheckAndFinish();
            }

            void CheckAndFinish() {
                if (!AnswersRemain) {
                    if (!Request.IsIndexOnly) {
                        for (size_t i = 0, count = Response->ResponseSz; i < count; ++i) {
                            const auto& item = Response->Responses[i];
                            if (item.Status == NKikimrProto::OK) {
                                Y_VERIFY_S(item.Buffer.size() == item.RequestedSize ? Min(item.RequestedSize,
                                    item.Id.BlobSize() - Min(item.Id.BlobSize(), item.Shift)) : item.Id.BlobSize(),
                                    "Id# " << item.Id << " Shift# " << item.Shift << " RequestedSize# " << item.RequestedSize
                                    << " Buffer.size# " << item.Buffer.size());
                            }
                        }
                    }
                    EndWithSuccess();
                }
            }

            void EndWithSuccess() {
                TraceResponse(std::nullopt);
                TBlobStorageQuery::EndWithSuccess(std::move(Response));
            }

            void EndWithError(NKikimrProto::EReplyStatus status, const TString& errorReason) {
                TraceResponse(status);
                TBlobStorageQuery::EndWithError(status, errorReason);
            }

            void TraceResponse(std::optional<NKikimrProto::EReplyStatus> status) {
                if (IS_LOG_PRIORITY_ENABLED(*TlsActivationContext, NLog::PRI_TRACE, NKikimrServices::BLOB_DEPOT_EVENTS)) {
                    for (ui32 i = 0; i < Response->ResponseSz; ++i) {
                        const auto& r = Response->Responses[i];
                        BDEV_QUERY(BDEV20, "TEvGet_end", (BlobId, r.Id), (Shift, r.Shift),
                            (RequestedSize, r.RequestedSize), (Status, status.value_or(r.Status)),
                            (Buffer.size, r.Buffer.size()));
                    }
                }
            }

            void ProcessResponse(ui64 /*id*/, TRequestContext::TPtr context, TResponse response) override {
                if (auto *p = std::get_if<TKeyResolved>(&response)) {
                    ProcessSingleResult(context->Obtain<TResolveKeyContext>().QueryIdx, p->ValueChain, p->ErrorReason);
                    CheckAndFinish();
                } else if (auto *p = std::get_if<TEvBlobStorage::TEvGetResult*>(&response)) {
                    Agent.HandleGetResult(context, **p);
                } else if (std::holds_alternative<TTabletDisconnected>(response)) {
                    if (auto *resolveContext = dynamic_cast<TResolveKeyContext*>(context.get())) {
                        STLOG(PRI_DEBUG, BLOB_DEPOT_AGENT, BDA26, "TTabletDisconnected", (AgentId, Agent.LogId),
                            (QueryId, GetQueryId()), (QueryIdx, resolveContext->QueryIdx));
                        Response->Responses[resolveContext->QueryIdx].Status = NKikimrProto::ERROR;
                        --AnswersRemain;
                        CheckAndFinish();
                    }
                } else {
                    Y_FAIL();
                }
            }

            ui64 GetTabletId() const override {
                ui64 value = 0;
                for (ui32 i = 0; i < Request.QuerySize; ++i) {
                    auto& req = Request.Queries[i];
                    if (value && value != req.Id.TabletID()) {
                        return 0;
                    }
                    value = req.Id.TabletID();
                }
                return value;
            }
        };

        return new TGetQuery(*this, std::move(ev));
    }

} // NKikimr::NBlobDepot
