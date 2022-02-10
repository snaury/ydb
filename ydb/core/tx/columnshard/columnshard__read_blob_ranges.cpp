#include "columnshard_impl.h"
#include "columnshard_txs.h"
#include "columnshard_schema.h"
#include "blob_manager_db.h"

namespace NKikimr::NColumnShard { 

using namespace NTabletFlatExecutor;

// Returns false in case of page fault
bool TryReadValue(NIceDb::TNiceDb& db, const TString& key, TString& value, ui32& readStatus) {
    auto rowset = db.Table<Schema::SmallBlobs>().Key(key).Select<Schema::SmallBlobs::Data>();
    if (!rowset.IsReady()) {
        return false;
    }

    if (rowset.IsValid()) {
        readStatus = NKikimrProto::EReplyStatus::OK;
        value = rowset.GetValue<Schema::SmallBlobs::Data>();
    } else {
        readStatus = NKikimrProto::EReplyStatus::NODATA;
        value.clear();
    }
    return true;
}

bool TTxReadBlobRanges::Execute(TTransactionContext& txc, const TActorContext& ctx) {
    Y_VERIFY(Ev);
    auto& record = Ev->Get()->Record;
    LOG_S_DEBUG("TTxReadBlobRanges.Execute at tablet " << Self->TabletID()<< " : " << record);

    Result = std::make_unique<TEvColumnShard::TEvReadBlobRangesResult>(Self->TabletID());

    NIceDb::TNiceDb db(txc.DB);

    ui64 successCount = 0;
    ui64 errorCount = 0;
    ui64 byteCount = 0;
    for (const auto& range : record.GetBlobRanges()) {
        auto blobId = range.GetBlobId();

        TString blob;
        ui32 status = NKikimrProto::EReplyStatus::NODATA;
        if (!TryReadValue(db, blobId, blob, status)) {
            return false; // Page fault
        }

        if (status == NKikimrProto::EReplyStatus::NODATA) {
            // If the value wasn't found by string key then try to parse the key as small blob id
            // and try lo lookup by this id serialized in the old format and in the new format
            TString error;
            NOlap::TUnifiedBlobId smallBlobId = NOlap::TUnifiedBlobId::ParseFromString(blobId, nullptr, error);

            if (smallBlobId.IsValid()) {
                if (!TryReadValue(db, smallBlobId.ToStringNew(), blob, status)) {
                    return false; // Page fault
                }

                if (status == NKikimrProto::EReplyStatus::NODATA &&
                    !TryReadValue(db, smallBlobId.ToStringLegacy(), blob, status))
                {
                    return false; // Page fault
                }
            }
        }

        auto* res = Result->Record.AddResults();
        res->MutableBlobRange()->CopyFrom(range);
        if (status == NKikimrProto::EReplyStatus::OK) {
            if (range.GetOffset() + range.GetSize() <= blob.size()) {
                res->SetData(blob.substr(range.GetOffset(), range.GetSize()));
                byteCount += range.GetSize();
            } else {
                LOG_S_NOTICE("TTxReadBlobRanges.Execute at tablet " << Self->TabletID()
                    << " the requested range " << range << " is outside blob data, blob size << " << blob.size());
                status = NKikimrProto::EReplyStatus::ERROR;
            }
        }
        res->SetStatus(status);
        if (status == NKikimrProto::EReplyStatus::OK) {
            ++successCount;
        } else {
            ++errorCount;
        }
    }

    // Sending result right away without waiting for Complete()
    // It is ok because the blob ids that were requested can only be known
    // to the caller if they have been already committed.
    ctx.Send(Ev->Sender, Result.release(), 0, Ev->Cookie);

    Self->IncCounter(COUNTER_SMALL_BLOB_READ_SUCCESS, successCount);
    Self->IncCounter(COUNTER_SMALL_BLOB_READ_ERROR, errorCount);
    Self->IncCounter(COUNTER_SMALL_BLOB_READ_BYTES, byteCount);

    return true;
}

void TTxReadBlobRanges::Complete(const TActorContext& ctx) {
    Y_UNUSED(ctx);
    LOG_S_DEBUG("TTxReadBlobRanges.Complete at tablet " << Self->TabletID());
}

} 
