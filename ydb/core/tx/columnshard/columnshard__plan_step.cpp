#include "columnshard_impl.h" 
#include "columnshard_txs.h" 
#include "columnshard_schema.h" 
 
#include <util/string/vector.h>

namespace NKikimr::NColumnShard { 
 
using namespace NTabletFlatExecutor; 
 
bool TTxPlanStep::Execute(TTransactionContext& txc, const TActorContext&) { 
    Y_VERIFY(Ev); 
    LOG_S_DEBUG("TTxPlanStep.Execute at tablet " << Self->TabletID()); 
 
    txc.DB.NoMoreReadsForTx(); 
    NIceDb::TNiceDb db(txc.DB); 
 
    auto& record = Ev->Get()->Record; 
    ui64 step = record.GetStep(); 
 
    TVector<ui64> txIds;
    for (const auto& tx : record.GetTransactions()) { 
        Y_VERIFY(tx.HasTxId());
        Y_VERIFY(tx.HasAckTo());
 
        txIds.push_back(tx.GetTxId());

        TActorId txOwner = ActorIdFromProto(tx.GetAckTo());
        TxAcks[txOwner].push_back(tx.GetTxId());
    }

    size_t plannedCount = 0;
    if (step > Self->LastPlannedStep) {
        ui64 lastTxId = 0;
        for (ui64 txId : txIds) {
            Y_VERIFY(lastTxId < txId, "Transactions must be sorted and unique");
            auto it = Self->BasicTxInfo.find(txId);
            if (it != Self->BasicTxInfo.end()) {
                if (it->second.PlanStep == 0) {
                    it->second.PlanStep = step;
                    Schema::UpdateTxInfoPlanStep(db, txId, step);
                    Self->PlanQueue.emplace(step, txId);
                    if (it->second.MaxStep != Max<ui64>()) {
                        Self->DeadlineQueue.erase(TColumnShard::TDeadlineQueueItem(it->second.MaxStep, txId));
                    }
                    ++plannedCount;
                } else {
                    LOG_S_WARN("Ignoring step " << step 
                        << " for txId " << txId
                        << " which is already planned for step " << step
                        << " at tablet " << Self->TabletID());
                }
            } else {
                LOG_S_WARN("Ignoring step " << step 
                    << " for unknown txId " << txId
                    << " at tablet " << Self->TabletID());
            }
            lastTxId = txId;
        } 
        Self->LastPlannedStep = step;
        Self->LastPlannedTxId = lastTxId;
        Schema::SaveSpecialValue(db, Schema::EValueIds::LastPlannedStep, Self->LastPlannedStep);
        Schema::SaveSpecialValue(db, Schema::EValueIds::LastPlannedTxId, Self->LastPlannedTxId);
        Self->RescheduleWaitingReads();
    } else {
        LOG_S_ERROR("Ignore old txIds [" 
            << JoinStrings(txIds.begin(), txIds.end(), ", ")
            << "] for step " << step
            << " last planned step " << Self->LastPlannedStep
            << " at tablet " << Self->TabletID());
    } 
 
    Result = std::make_unique<TEvTxProcessing::TEvPlanStepAccepted>(Self->TabletID(), step);
 
    Self->IncCounter(COUNTER_PLAN_STEP_ACCEPTED); 

    if (plannedCount > 0 || Self->HaveOutdatedTxs()) { 
        Self->EnqueueProgressTx(); 
    }
    return true; 
} 
 
void TTxPlanStep::Complete(const TActorContext& ctx) { 
    Y_VERIFY(Ev); 
    Y_VERIFY(Result); 
    LOG_S_DEBUG("TTxPlanStep.Complete at tablet " << Self->TabletID()); 
 
    ui64 step = Ev->Get()->Record.GetStep();
    for (auto& kv : TxAcks) {
        ctx.Send(kv.first, new TEvTxProcessing::TEvPlanStepAck(Self->TabletID(), step, kv.second.begin(), kv.second.end()));
    }

    ctx.Send(Ev->Sender, Result.release()); 
} 
 
} 
