#include "executor_pool_io.h"
#include "actor.h"
#include "config.h"
#include "mailbox.h"
#include <ydb/library/actors/util/affinity.h>
#include <ydb/library/actors/util/datetime.h>

namespace NActors {
    TIOExecutorPool::TIOExecutorPool(ui32 poolId, ui32 threads, const TString& poolName, TAffinity* affinity, bool /*useRingQueue*/)
        : TExecutorPoolBaseMailboxed(poolId)
        , PoolThreads(threads)
        , ThreadsAffinity(affinity)
        , Threads(new TExecutorThreadCtx[threads])
        , PoolName(poolName)
    {}

    TIOExecutorPool::TIOExecutorPool(const TIOExecutorPoolConfig& cfg, IHarmonizer *harmonizer)
        : TIOExecutorPool(
            cfg.PoolId,
            cfg.Threads,
            cfg.PoolName,
            new TAffinity(cfg.Affinity),
            cfg.UseRingQueue
        )
    {
        Harmonizer = harmonizer;
    }

    TIOExecutorPool::~TIOExecutorPool() {
        Threads.Destroy();
    }

    void TIOExecutorPool::PushActivation(TMailbox* m) {
        if (RunQueue.Push(m)) {
            if (RunQueueWaiters.load() > 0) {
                // Run queue was empty, we need to wake at least one thread
                std::unique_lock g(RunQueueLock);
                RunQueueWake.notify_one();
            }
        }
    }

    TMailbox* TIOExecutorPool::NextActivationLocked() {
        auto [m, wake] = RunQueue.Pop();
        if (wake && RunQueueWaiters.load() > 0) {
            // Run queue has more items, we need to wake the next thread
            RunQueueWake.notify_one();
        }
        return m;
    }

    TMailbox* TIOExecutorPool::GetReadyActivation(TWorkerContext& wctx, ui64) {
        i16 workerId = wctx.WorkerId;
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);

        std::unique_lock g(RunQueueLock);
        while (!StopFlag) {
            if (TMailbox* m = NextActivationLocked()) {
                return m;
            }
            ++RunQueueWaiters;
            if (TMailbox* m = NextActivationLocked()) {
                --RunQueueWaiters;
                return m;
            }
            RunQueueWake.wait(g);
            --RunQueueWaiters;
        }

        return nullptr;
    }

    void TIOExecutorPool::Schedule(TInstant deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Schedule(deadline - ActorSystem->Timestamp(), ev, cookie, workerId);
    }

    void TIOExecutorPool::Schedule(TMonotonic deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_DEBUG_ABORT_UNLESS(workerId >= 0);
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);
        const auto current = ActorSystem->Monotonic();
        if (deadline < current)
            deadline = current;

        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TIOExecutorPool::Schedule(TDuration delta, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_DEBUG_ABORT_UNLESS(workerId >= 0);
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);
        const auto deadline = ActorSystem->Monotonic() + delta;

        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TIOExecutorPool::ScheduleActivationEx(TMailbox* mailbox, ui64) {
        PushActivation(mailbox);
    }

    void TIOExecutorPool::ScheduleActivation(TMailbox* mailbox) {
        PushActivation(mailbox);
    }

    void TIOExecutorPool::SpecificScheduleActivation(TMailbox* mailbox) {
        PushActivation(mailbox);
    }

    TAffinity* TIOExecutorPool::Affinity() const {
        return ThreadsAffinity.Get();
    }

    ui32 TIOExecutorPool::GetThreads() const {
        return PoolThreads;
    }

    void TIOExecutorPool::Prepare(TActorSystem* actorSystem, NSchedulerQueue::TReader** scheduleReaders, ui32* scheduleSz) {
        TAffinityGuard affinityGuard(Affinity());

        ActorSystem = actorSystem;

        ScheduleReaders.Reset(new NSchedulerQueue::TReader[PoolThreads]);
        ScheduleWriters.Reset(new NSchedulerQueue::TWriter[PoolThreads]);

        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread.reset(new TExecutorThread(i, 0, actorSystem, this, MailboxTable.Get(), PoolName));
            ScheduleWriters[i].Init(ScheduleReaders[i]);
        }

        *scheduleReaders = ScheduleReaders.Get();
        *scheduleSz = PoolThreads;
    }

    void TIOExecutorPool::Start() {
        TAffinityGuard affinityGuard(Affinity());

        for (i16 i = 0; i != PoolThreads; ++i)
            Threads[i].Thread->Start();
    }

    void TIOExecutorPool::PrepareStop() {
        std::unique_lock g(RunQueueLock);
        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread->StopFlag = true;
            Threads[i].WaitingPad.Interrupt();
        }
        StopFlag = true;
        RunQueueWake.notify_all();
    }

    void TIOExecutorPool::Shutdown() {
        for (i16 i = 0; i != PoolThreads; ++i)
            Threads[i].Thread->Join();
    }

    void TIOExecutorPool::GetCurrentStats(TExecutorPoolStats& poolStats, TVector<TExecutorThreadStats>& statsCopy) const {
        poolStats.CurrentThreadCount = PoolThreads;
        poolStats.DefaultThreadCount = PoolThreads;
        poolStats.MaxThreadCount = PoolThreads;
        poolStats.PotentialMaxThreadCount = PoolThreads;
        statsCopy.resize(PoolThreads + 1);
        // Save counters from the pool object
        statsCopy[0] = TExecutorThreadStats();
        statsCopy[0].Aggregate(Stats);
        // Per-thread stats
        for (i16 i = 0; i < PoolThreads; ++i) {
            Threads[i].Thread->GetCurrentStats(statsCopy[i + 1]);
        }
    }

    void TIOExecutorPool::GetExecutorPoolState(TExecutorPoolState &poolState) const {
        if (Harmonizer) {
            TPoolHarmonizerStats stats = Harmonizer->GetPoolStats(PoolId);
            poolState.UsedCpu = stats.AvgConsumedCpu;
        }
        poolState.CurrentLimit = PoolThreads;
        poolState.MaxLimit = PoolThreads;
        poolState.MinLimit = PoolThreads;
        poolState.PossibleMaxLimit = PoolThreads;
    }

    TString TIOExecutorPool::GetName() const {
        return PoolName;
    }
}
