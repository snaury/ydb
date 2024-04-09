#pragma once

#include "actorsystem.h"
#include "executor_pool_base.h"
#include <ydb/library/actors/util/spinparkpad.h>

namespace NActors {

    class TWorkStealingExecutorPool : public TExecutorPoolBaseMailboxed {
    public:
        explicit TWorkStealingExecutorPool(const TWorkStealingExecutorPoolConfig& cfg, IHarmonizer* harmonizer = nullptr);
        ~TWorkStealingExecutorPool();

        void ScheduleActivation(TMailbox* mailbox) override;
        void SpecificScheduleActivation(TMailbox* mailbox) override;
        void ScheduleActivationEx(TMailbox* mailbox, ui64 revolvingCounter) override;
        TAffinity* Affinity() const override;
        ui32 GetThreads() const override;

        TMailbox* GetReadyActivation(TWorkerContext& wctx, ui64 revolvingCounter) override;

        void Schedule(TInstant deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;
        void Schedule(TMonotonic deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;
        void Schedule(TDuration delta, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) override;

        void Prepare(TActorSystem* actorSystem, NSchedulerQueue::TReader** scheduleReaders, ui32* scheduleSz) override;
        void Start() override;
        void PrepareStop() override;
        void Shutdown() override;

        void GetCurrentStats(TExecutorPoolStats& poolStats, TVector<TExecutorThreadStats>& statsCopy) const override;
        TString GetName() const override;

    private:
        static constexpr size_t MaxLocalTasks = 256;
        static constexpr ui32 TaskIndexMask = 255;

        struct alignas(64) TThreadCtx {
            THolder<TExecutorThread> Thread;
            size_t LocalProcessed = 0;
            bool Deferred = false;

            alignas(64) std::atomic<TMailbox*> LocalQueue[MaxLocalTasks];
            alignas(64) std::atomic<ui64> LocalQueueHeadTail{ 0 };

            alignas(64) TSpinParkPad WaitPad;
            ui32 NextWaitingThreadId = 0;
            bool Spinning = false;
            bool Waiting = false;
        };

    private:
        TMailbox* NextGlobal();
        TMailbox* NextGlobalLocked();

        void PushGlobal(TMailbox* mailbox);
        void PushGlobalBatch(TMailbox** buf, size_t count);

    private:
        TMailbox* NextLocal(TThreadCtx* state);
        TMailbox* TryStealing(TThreadCtx* state);
        TMailbox* TryStealingFrom(TThreadCtx* state, TThreadCtx* from);

        ui32 LocalQueueSize(TThreadCtx* state);
        bool PushLocal(TThreadCtx* state, TMailbox* mailbox);
        bool OffloadToGlobal(TThreadCtx* state);

    private:
        void WakeByNewWork();

    private:
        const TWorkStealingExecutorPoolConfig Config;
        const i16 PoolThreads;
        const TString PoolName;
        const TIntrusivePtr<TAffinity> ThreadsAffinity;

        TArrayHolder<TThreadCtx> Threads;
        TArrayHolder<NSchedulerQueue::TReader> ScheduleReaders;
        TArrayHolder<NSchedulerQueue::TWriter> ScheduleWriters;

        alignas(64) std::atomic<ui64> WaitBits{ 0 };

        alignas(64) std::mutex Mutex;
        TMailbox* GlobalQueueHead{ nullptr };
        TMailbox* GlobalQueueTail{ nullptr };

        alignas(64) std::atomic<TMailbox*> GlobalQueueNew{ nullptr };
    };

} // namespace NActors
