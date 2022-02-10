#include "datashard_txs.h" 
#include "datashard_failpoints.h"

namespace NKikimr {
namespace NDataShard {

TDataShard::TTxProgressTransaction::TTxProgressTransaction(TDataShard *self, TOperation::TPtr op)
    : TBase(self)
    , ActiveOp(std::move(op))
{} 

bool TDataShard::TTxProgressTransaction::Execute(TTransactionContext &txc, const TActorContext &ctx) {
    LOG_DEBUG_S(ctx, NKikimrServices::TX_DATASHARD,
                "TTxProgressTransaction::Execute at " << Self->TabletID());

    try {
        if (!Self->IsStateActive()) { 
            Self->IncCounter(COUNTER_TX_PROGRESS_SHARD_INACTIVE); 
            LOG_INFO_S(ctx, NKikimrServices::TX_DATASHARD, 
                "Progress tx at non-ready tablet " << Self->TabletID() << " state " << Self->State); 
            Y_VERIFY(!ActiveOp, "Unexpected ActiveOp at inactive shard %" PRIu64, Self->TabletID());
            Self->PlanQueue.Reset(ctx);
            return true;
        }

        NIceDb::TNiceDb db(txc.DB); 
 
        if (!ActiveOp) {
            const bool expireSnapshotsAllowed = (
                    Self->State == TShardState::Ready ||
                    Self->State == TShardState::SplitSrcWaitForNoTxInFlight ||
                    Self->State == TShardState::SplitSrcMakeSnapshot);

            const bool needFutureCleanup = (
                    Self->TxInFly() > 0 ||
                    (expireSnapshotsAllowed && Self->GetSnapshotManager().HasExpiringSnapshots()));

            if (needFutureCleanup) {
                Self->PlanCleanup(ctx);
            } 

            // Allow another concurrent progress tx
            Self->PlanQueue.Reset(ctx);
            Self->Pipeline.ActivateWaitingTxOps(ctx);

            ActiveOp = Self->Pipeline.GetNextActiveOp(false);
            if (!ActiveOp) {
                Self->IncCounter(COUNTER_TX_PROGRESS_IDLE); 
                LOG_INFO_S(ctx, NKikimrServices::TX_DATASHARD,
                           "No tx to execute at " << Self->TabletID() << " TxInFly " << Self->TxInFly());
                return true; 
            } 

            Y_VERIFY_S(!ActiveOp->IsInProgress(),
                       "GetNextActiveOp returned in-progress operation "
                       << ActiveOp->GetKind() << " " << *ActiveOp << " (unit "
                       << ActiveOp->GetCurrentUnit() << ") at " << Self->TabletID());
            ActiveOp->IncrementInProgress();
        }

        Y_VERIFY(ActiveOp && ActiveOp->IsInProgress());
        auto status = Self->Pipeline.RunExecutionPlan(ActiveOp, CompleteList, txc, ctx);

        if (Self->Pipeline.CanRunAnotherOp())
            Self->PlanQueue.Progress(ctx);

        switch (status) {
            case EExecutionStatus::Restart:
                // Restart even if current CompleteList is not empty
                // It will be extended in subsequent iterations
                return false;

            case EExecutionStatus::Reschedule:
                // Reschedule transaction as soon as possible
                if (!ActiveOp->IsExecutionPlanFinished()) {
                    ActiveOp->IncrementInProgress();
                    Self->ExecuteProgressTx(ActiveOp, ctx);
                    Rescheduled = true;
                }
                break;

            case EExecutionStatus::Executed:
            case EExecutionStatus::Continue:
            case EExecutionStatus::WaitComplete:
                // No special handling
                break;

            default:
                Y_FAIL_S("unexpected execution status " << status << " for operation "
                        << *ActiveOp << " " << ActiveOp->GetKind() << " at " << Self->TabletID());
        }

        if (!CompleteList.empty()) {
            // Keep operation active until we run the complete list
            CommitStart = AppData()->TimeProvider->Now();
        } else {
            // Release operation as it's no longer needed
            ActiveOp->DecrementInProgress();
            ActiveOp = nullptr;
        }

        // Commit all side effects
        return true;
    } catch (...) {
        Y_FAIL("there must be no leaked exceptions");
    }
}

void TDataShard::TTxProgressTransaction::Complete(const TActorContext &ctx) {
    LOG_DEBUG_S(ctx, NKikimrServices::TX_DATASHARD,
                "TTxProgressTransaction::Complete at " << Self->TabletID());

    if (ActiveOp) {
        Y_VERIFY(!ActiveOp->GetExecutionPlan().empty());
        if (!CompleteList.empty()) {
            auto commitTime = AppData()->TimeProvider->Now() - CommitStart;
            ActiveOp->SetCommitTime(CompleteList.front(), commitTime);

            if (!ActiveOp->IsExecutionPlanFinished()
                && (ActiveOp->GetCurrentUnit() != CompleteList.front()))
                ActiveOp->SetDelayedCommitTime(commitTime);

            Self->Pipeline.RunCompleteList(ActiveOp, CompleteList, ctx);
        }
        ActiveOp->DecrementInProgress();

        if (!ActiveOp->IsInProgress() && !ActiveOp->IsExecutionPlanFinished())
            Self->Pipeline.AddCandidateOp(ActiveOp);

        if (Self->Pipeline.CanRunAnotherOp())
            Self->PlanQueue.Progress(ctx);
    }

    Self->CheckSplitCanStart(ctx);
    Self->CheckMvccStateChangeCanStart(ctx);
}

}}
