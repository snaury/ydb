#pragma once

#include "actorsystem.h"
#include "executor_thread.h"
#include "executor_thread_ctx.h"
#include "harmonizer.h"
#include "scheduler_queue.h"
#include "executor_pool_base.h"
#include <ydb/library/actors/actor_type/indexes.h>
#include <ydb/library/actors/util/ticket_lock.h>
#include <ydb/library/actors/util/unordered_cache.h>
#include <ydb/library/actors/util/threadparkpad.h>
#include <util/system/condvar.h>

namespace NActors {
    struct TIOExecutorPoolConfig;

    class TIOExecutorPool: public TExecutorPoolBaseMailboxed {
        const i16 PoolThreads;
        TIntrusivePtr<TAffinity> ThreadsAffinity;

        TArrayHolder<TExecutorThreadCtx> Threads;

        TArrayHolder<NSchedulerQueue::TReader> ScheduleReaders;
        TArrayHolder<NSchedulerQueue::TWriter> ScheduleWriters;

        IHarmonizer *Harmonizer = nullptr;

        const TString PoolName;
        const ui32 ActorSystemIndex = NActors::TActorTypeOperator::GetActorSystemIndex();

        TMailboxRunQueue RunQueue;
        std::atomic<size_t> RunQueueWaiters{ 0 };
        std::mutex RunQueueLock;
        std::condition_variable RunQueueWake;
        bool StopFlag = false;

    public:
        TIOExecutorPool(ui32 poolId, ui32 threads, const TString& poolName = "", TAffinity* affinity = nullptr, bool useRingQueue = false);
        explicit TIOExecutorPool(const TIOExecutorPoolConfig& cfg, IHarmonizer *harmonizer = nullptr);
        ~TIOExecutorPool();

        TMailbox* GetReadyActivation(TWorkerContext& wctx, ui64 revolvingCounter) override;

        void Schedule(TInstant deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;
        void Schedule(TMonotonic deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;
        void Schedule(TDuration delta, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;

        void ScheduleActivation(TMailbox* mailbox) override;
        void SpecificScheduleActivation(TMailbox* mailbox) override;
        TAffinity* Affinity() const override;
        ui32 GetThreads() const override;

        void ScheduleActivationEx(TMailbox* mailbox, ui64 revolvingWriteCounter) override;

        void Prepare(TActorSystem* actorSystem, NSchedulerQueue::TReader** scheduleReaders, ui32* scheduleSz) override;
        void Start() override;
        void PrepareStop() override;
        void Shutdown() override;

        void GetCurrentStats(TExecutorPoolStats& poolStats, TVector<TExecutorThreadStats>& statsCopy) const override;
        void GetExecutorPoolState(TExecutorPoolState &poolState) const override;
        TString GetName() const override;

    private:
        void PushActivation(TMailbox* m);
        TMailbox* NextActivationLocked();
    };
}
