#include "executor_pool_work_stealing.h"
#include "executor_pool_basic_feature_flags.h"
#include "actor.h"

namespace NActors {

    namespace {
        TMailbox* GetNextPtr(TMailbox* mailbox) {
            return reinterpret_cast<TMailbox*>(mailbox->NextRunPtr.load(std::memory_order_relaxed));
        }

        void SetNextPtr(TMailbox* mailbox, TMailbox* next) {
            mailbox->NextRunPtr.store(reinterpret_cast<uintptr_t>(next), std::memory_order_relaxed);
        }

        inline ui64 PackHeadTail(ui32 head, ui32 tail) noexcept {
            return (ui64(head) << 32) | tail;
        }

        inline std::tuple<ui32, ui32> UnpackHeadTail(ui64 value) noexcept {
            return { ui32(value >> 32), ui32(value) };
        }

        // We don't need to wake up threads as long as there's at least one spinning thread
        static constexpr int SpinningThreadsShift = 0;
        static constexpr ui64 SpinningThreadsMask = 0xFFFFu;

        // A linked list of threads waiting on their WaitPad, empty when zero
        static constexpr int WaitingThreadsShift = 16;
        static constexpr ui64 WaitingThreadsMask = 0xFFFFu;

        // This flag is set when new work is added with no spinning or waiting threads
        // The first thread that tries to park must become spinning instead
        static constexpr ui64 FlagNeedSpinning = ui64(1) << 32;

        // This flag is set when actor system is shutting down
        // Threads that are out work must stop instead of parking
        static constexpr ui64 FlagNeedShutdown = ui64(1) << 33;

        inline ui32 GetSpinningThreads(ui64 current) noexcept {
            return (current >> SpinningThreadsShift) & SpinningThreadsMask;
        }

        inline ui64 IncrementSpinningThreads(ui64 current) noexcept {
            return current + (ui64(1) << SpinningThreadsShift);
        }

        inline ui64 DecrementSpinningThreads(ui64 current) noexcept {
            return current - (ui64(1) << SpinningThreadsShift);
        }

        inline ui32 GetWaitingThreadId(ui64 current) noexcept {
            return (current >> WaitingThreadsShift) & WaitingThreadsMask;
        }

        inline ui64 ChangeWaitingThreadId(ui64 current, ui32 threadId) noexcept {
            return (current & ~(WaitingThreadsMask << WaitingThreadsShift))
                | (ui64(threadId) << WaitingThreadsShift);
        }
    }

    TWorkStealingExecutorPool::TWorkStealingExecutorPool(const TWorkStealingExecutorPoolConfig& cfg, IHarmonizer* harmonizer)
        : TExecutorPoolBaseMailboxed(cfg.PoolId)
        , Config(cfg)
        , PoolThreads(cfg.Threads)
        , ThreadsAffinity(new TAffinity(cfg.Affinity))
        , Threads(new TThreadCtx[cfg.Threads])
    {
        Y_UNUSED(harmonizer);
    }

    TWorkStealingExecutorPool::~TWorkStealingExecutorPool() {
    }

    void TWorkStealingExecutorPool::ScheduleActivation(TMailbox* mailbox) {
        if (TlsThreadContext && TlsThreadContext->Pool == this && TlsThreadContext->WorkerId >= 0) {
            auto* state = &Threads[TlsThreadContext->WorkerId];

            while (!PushLocal(state, mailbox)) {
                OffloadToGlobal(state);
            }

            WakeByNewWork();
        } else {
            PushGlobal(mailbox);
        }
    }

    void TWorkStealingExecutorPool::SpecificScheduleActivation(TMailbox* mailbox) {
        return ScheduleActivation(mailbox);
    }

    void TWorkStealingExecutorPool::ScheduleActivationEx(TMailbox* mailbox, ui64) {
        ScheduleActivation(mailbox);
    }

    TAffinity* TWorkStealingExecutorPool::Affinity() const {
        return ThreadsAffinity.Get();
    }

    ui32 TWorkStealingExecutorPool::GetThreads() const {
        return PoolThreads;
    }

    TMailbox* TWorkStealingExecutorPool::GetReadyActivation(TWorkerContext& wctx, ui64) {
        TWorkerId workerId = wctx.WorkerId;
        if (workerId < 0) {
            // Shared pool thread?
            return NextGlobal();
        }

        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);

        auto* state = &Threads[workerId];

        bool checkedGlobal = false;

        if (state->LocalProcessed >= 61) {
            state->LocalProcessed = 0;
            if (TMailbox* mailbox = NextGlobal()) {
                if (state->Deferred) {
                    if (LocalQueueSize(state) > 0) {
                        WakeByNewWork();
                    }
                    state->Deferred = false;
                }
                return mailbox;
            }
            checkedGlobal = true;
        }

        if (TMailbox* mailbox = NextLocal(state)) {
            state->LocalProcessed++;
            return mailbox;
        }

        if (TMailbox* mailbox = TryStealing(state)) {
            state->LocalProcessed++;
            return mailbox;
        }

        if (!checkedGlobal) {
            if (TMailbox* mailbox = NextGlobal()) {
                return mailbox;
            }
        }

        ui64 current = WaitBits.load();
        auto tryStopSpinning = [&](bool foundWork) -> bool {
            Y_ABORT_UNLESS(state->Spinning);
            Y_ABORT_UNLESS(GetSpinningThreads(current) > 0);
            if (GetSpinningThreads(current) > 1 || (current & FlagNeedSpinning)) {
                // Let other spinning threads deal with wakeups
                ui64 updated = DecrementSpinningThreads(current);
                if (WaitBits.compare_exchange_strong(current, updated)) {
                    state->Spinning = false;
                    return true;
                } else {
                    return false;
                }
            }
            Y_ABORT_UNLESS(GetSpinningThreads(current) == 1 && !(current & FlagNeedSpinning));
            ui64 threadId = GetWaitingThreadId(current);
            if (foundWork && threadId) {
                // Try to wake the next thread
                TThreadCtx* other = &Threads[threadId - 1];
                ui64 updated = ChangeWaitingThreadId(current, other->NextWaitingThreadId);
                if (WaitBits.compare_exchange_strong(current, updated)) {
                    state->Spinning = false;
                    Y_ABORT_UNLESS(other->Waiting);
                    other->Waiting = false;
                    other->Spinning = true;
                    other->WaitPad.Unpark();
                    return true;
                }
            } else {
                // Try to just remove ourselves from spinning
                ui64 updated = DecrementSpinningThreads(current);
                if (WaitBits.compare_exchange_strong(current, updated)) {
                    state->Spinning = false;
                    return true;
                }
            }
            // Lost the race
            return false;
        };

        for (;;) {
            if (state->Spinning) {
                TMailbox* mailbox;
                {
                    std::unique_lock g(Mutex);
                    mailbox = NextGlobalLocked();
                }

                if (!mailbox) {
                    mailbox = TryStealing(state);
                    if (mailbox) {
                        state->LocalProcessed++;
                    }
                }

                current = WaitBits.load();

                if (mailbox) {
                    // Remove ourselves from spinning threads and return
                    while (!tryStopSpinning(true)) {
                        // We must keep trying
                    }
                    return mailbox;
                }

                bool isLast = GetSpinningThreads(current) == 1 && !(current & FlagNeedSpinning);
                if (!tryStopSpinning(false)) {
                    // Lost the race, recheck queues
                    Y_ABORT_UNLESS(state->Spinning);
                    continue;
                }

                Y_ABORT_UNLESS(!state->Spinning);

                if (isLast) {
                    // We have to recheck queues, another thread might have added work
                    {
                        std::unique_lock g(Mutex);
                        mailbox = NextGlobalLocked();
                    }

                    if (!mailbox) {
                        mailbox = TryStealing(state);
                        if (mailbox) {
                            state->LocalProcessed++;
                        }
                    }

                    if (mailbox) {
                        return mailbox;
                    }
                }
            }

            Y_ABORT_UNLESS(!state->Spinning);

            if (current & FlagNeedShutdown) {
                return nullptr;
            }

            if ((current & FlagNeedSpinning) && GetWaitingThreadId(current) == 0) {
                // Work added concurrently and no waiting threads, try to become a spinning thread
                Y_ABORT_UNLESS(GetSpinningThreads(current) == 0);
                ui64 updated = IncrementSpinningThreads(current & ~FlagNeedSpinning);
                if (WaitBits.compare_exchange_strong(current, updated)) {
                    // We are now the spinning thread
                    state->Spinning = true;
                    current = updated;
                    continue;
                }
            } else {
                // Insert ourselves to the waiting list
                state->Waiting = true;
                state->NextWaitingThreadId = GetWaitingThreadId(current);
                ui64 updated = ChangeWaitingThreadId(current, workerId + 1);
                if (WaitBits.compare_exchange_strong(current, updated)) {
                    bool interrupted = state->WaitPad.Park();
                    Y_ABORT_UNLESS(!state->Waiting);
                    if (interrupted) {
                        Y_ABORT_UNLESS(!state->Spinning);
                        return nullptr;
                    }
                    // We are now the spinning thread
                    Y_ABORT_UNLESS(state->Spinning);
                    current = WaitBits.load();
                    Y_ABORT_UNLESS(GetSpinningThreads(current) > 0);
                    continue;
                }
                state->NextWaitingThreadId = 0;
                state->Waiting = false;
            }

            // Lost the race, try again
        }
    }

    TMailbox* TWorkStealingExecutorPool::NextGlobal() {
        // Avoid locking the mutex when there is no work in the global queue
        if (GlobalQueueNew.load(std::memory_order_acquire)) {
            std::unique_lock g(Mutex);
            return NextGlobalLocked();
        }
        return nullptr;
    }

    TMailbox* TWorkStealingExecutorPool::NextGlobalLocked() {
        TMailbox* last = GlobalQueueNew.load(std::memory_order_acquire);
        while (last) {
            if (TMailbox* newTail = GetNextPtr(last)) {
                SetNextPtr(last, nullptr);
                TMailbox* newHead = newTail;
                TMailbox* next = nullptr;
                while (TMailbox* prev = GetNextPtr(newHead)) {
                    SetNextPtr(newHead, next);
                    next = newHead;
                    newHead = prev;
                }
                SetNextPtr(newHead, next);
                if (GlobalQueueTail) {
                    SetNextPtr(GlobalQueueTail, newHead);
                    GlobalQueueTail = newTail;
                } else {
                    GlobalQueueHead = newHead;
                    GlobalQueueTail = newTail;
                }
                break;
            }

            if (GlobalQueueHead) {
                break;
            }

            if (GlobalQueueNew.compare_exchange_strong(last, nullptr, std::memory_order_acquire)) {
                GlobalQueueHead = last;
                GlobalQueueTail = last;
                break;
            }
        }

        if (!GlobalQueueHead) {
            return nullptr;
        }

        TMailbox* m = GlobalQueueHead;
        GlobalQueueHead = GetNextPtr(m);
        if (!GlobalQueueHead) {
            GlobalQueueTail = nullptr;
        }

        return m;
    }

    void TWorkStealingExecutorPool::PushGlobal(TMailbox* mailbox) {
        TMailbox* current = GlobalQueueNew.load(std::memory_order_relaxed);
        for (;;) {
            SetNextPtr(mailbox, current);
            if (GlobalQueueNew.compare_exchange_weak(current, mailbox, std::memory_order_release)) {
                break;
            }
        }
        WakeByNewWork();
    }

    void TWorkStealingExecutorPool::PushGlobalBatch(TMailbox** buf, size_t count) {
        Y_ABORT_UNLESS(count > 0);
        TMailbox* tail = buf[0];
        TMailbox* head = buf[0];
        for (size_t i = 1; i < count; ++i) {
            SetNextPtr(buf[i], buf[i-1]);
            head = buf[i];
        }

        TMailbox* current = GlobalQueueNew.load(std::memory_order_relaxed);
        for (;;) {
            SetNextPtr(tail, current);
            if (GlobalQueueNew.compare_exchange_weak(current, head, std::memory_order_release)) {
                break;
            }
        }
        WakeByNewWork();
    }

    TMailbox* TWorkStealingExecutorPool::NextLocal(TThreadCtx* state) {
        auto head_tail = state->LocalQueueHeadTail.load(std::memory_order_relaxed);
        for (;;) {
            auto [head, tail] = UnpackHeadTail(head_tail);
            if (head == tail) {
                return nullptr;
            }
            auto* r = state->LocalQueue[head & TaskIndexMask].load(std::memory_order_relaxed);
            // Note: we don't need any synchronization here, because only the
            // current thread ever writes to the local queue and reordering in
            // the current thread is not a problem.
            if (state->LocalQueueHeadTail.compare_exchange_strong(head_tail,
                    PackHeadTail(head + 1, tail), std::memory_order_relaxed))
            {
                return r;
            }
            // Lost the race with a stealer, head_tail is reloaded
        }
    }

    TMailbox* TWorkStealingExecutorPool::TryStealing(TThreadCtx* state) {
        ui32 pos = Random();
        for (i16 i = 0; i < PoolThreads; ++i, pos += 1) {
            TThreadCtx* from = &Threads[pos % PoolThreads];
            if (state != from) {
                if (auto* r = TryStealingFrom(state, from)) {
                    return r;
                }
            }
        }
        return nullptr;
    }

    TMailbox* TWorkStealingExecutorPool::TryStealingFrom(TThreadCtx* state, TThreadCtx* from) {
        auto our_head_tail = state->LocalQueueHeadTail.load(std::memory_order_relaxed);
        auto [our_head, our_tail] = UnpackHeadTail(our_head_tail);
        assert(our_head == our_tail);
        // Note: acquire synchronizes with PushLocal
        // Note: we use seq_cst to establish total order with FlagLocalWork
        auto their_head_tail = from->LocalQueueHeadTail.load();
        for (;;) {
            auto [their_head, their_tail] = UnpackHeadTail(their_head_tail);
            uint32_t n = their_tail - their_head;
            if (n == 0) {
                return nullptr;
            }
            n -= n >> 1;
            // Copy pointer values, the first task will be returned
            auto* r = from->LocalQueue[their_head & TaskIndexMask].load(std::memory_order_relaxed);
            for (uint32_t i = 1; i < n; ++i) {
                auto* more = from->LocalQueue[(their_head + i) & TaskIndexMask].load(std::memory_order_relaxed);
                state->LocalQueue[(our_tail + i - 1) & TaskIndexMask].store(more, std::memory_order_relaxed);
            }
            // Note: acquire needed for reload on failures
            // Note: release needed so loads above are not reordered after cas
            // Note: we use seq_cst to establish total order here
            if (from->LocalQueueHeadTail.compare_exchange_strong(their_head_tail,
                    PackHeadTail(their_head + n, their_tail)))
            {
                // We successfully stole some tasks
                if (n > 1) {
                    // Note: we can use a store, because local queue was empty
                    // and no concurrent stealer could have changed head/tail
                    // Note: release synchronizes with other threads stealing our tasks
                    // Note: we use seq_cst to establish total order here
                    state->LocalQueueHeadTail.store(
                        PackHeadTail(our_head, our_tail + n - 1));
                }
                return r;
            }
            // Lost the race: will retry with updated their_head_tail
        }
    }

    ui32 TWorkStealingExecutorPool::LocalQueueSize(TThreadCtx* state) {
        auto head_tail = state->LocalQueueHeadTail.load(std::memory_order_relaxed);
        auto [head, tail] = UnpackHeadTail(head_tail);
        return tail - head;
    }

    bool TWorkStealingExecutorPool::PushLocal(TThreadCtx* state, TMailbox* r) {
        // Note: tail is only modified locally, no synchronization needed
        // And while head may be updated by a stealer, we only use it for
        // determining if there's enough capacity, nothing else.
        auto head_tail = state->LocalQueueHeadTail.load(std::memory_order_relaxed);
        auto [head, tail] = UnpackHeadTail(head_tail);
        if (uint32_t(tail - head) >= MaxLocalTasks) {
            return false;
        }
        state->LocalQueue[tail & TaskIndexMask].store(r, std::memory_order_relaxed);
        // We want to increment tail without changing head as a single increment
        // This computes a wrapping difference that will make tail = tail + 1
        // even when other threads are modifying head concurrently.
        uint64_t increment = PackHeadTail(0, tail + 1) - PackHeadTail(0, tail);
        // Note: release synchronizes with other threads stealing tasks
        // Note: we use seq_cst to establish total order with FlagLocalWork
        state->LocalQueueHeadTail.fetch_add(increment);
        return true;
    }

    bool TWorkStealingExecutorPool::OffloadToGlobal(TThreadCtx* state) {
        auto head_tail = state->LocalQueueHeadTail.load(std::memory_order_relaxed);
        for (;;) {
            auto [head, tail] = UnpackHeadTail(head_tail);
            uint32_t n = tail - head;
            if (n < MaxLocalTasks) {
                break;
            }
            n = MaxLocalTasks / 2;
            TMailbox* tasks[MaxLocalTasks / 2];
            for (uint32_t i = 0; i < n; ++i) {
                tasks[i] = state->LocalQueue[(head + i) & TaskIndexMask].load(std::memory_order_relaxed);
            }
            // Note: we don't need any synchronization here, because only the
            // current thread ever writes to the local queue and reordering in
            // the current thread is not a problem.
            if (state->LocalQueueHeadTail.compare_exchange_strong(head_tail,
                    PackHeadTail(head + n, tail), std::memory_order_relaxed))
            {
                // Importantly, we must push these tasks to the end of the
                // global queue, and not the front, even though they would
                // have executed very soon. Local queue is usually checked
                // first, but we avoid global queue starvation by checking
                // it periodically even when local tasks are available. A
                // subtle point is that tasks currently in the local queue
                // may have been added unfairly, ahead of much older tasks
                // in the global queue. Pushing them to the front of the
                // global queue then may allow a bunch of rescheduling
                // tasks to completely starve the global queue.
                PushGlobalBatch(tasks, n);
                return true;
            }
            // Lost the race with a stealer, head_tail is reloaded
        }
        return false;
    }

    void TWorkStealingExecutorPool::WakeByNewWork() {
        auto current = WaitBits.load();
        for (;;) {
            if (GetSpinningThreads(current) > 0 || (current & FlagNeedSpinning)) {
                // There are spinning threads already
                return;
            }

            auto updated = (current | FlagNeedSpinning);
            if (WaitBits.compare_exchange_weak(current, updated)) {
                current = updated;
                // Try to claim a waiting thread while removing FlagNeedSpinning
                // There's no risk of ABA since only the thread that transfers
                // spin is allowed to remove threads from the wait list.
                while (ui32 threadId = GetWaitingThreadId(current)) {
                    TThreadCtx* state = &Threads[threadId - 1];
                    updated = IncrementSpinningThreads(
                        ChangeWaitingThreadId(current & ~FlagNeedSpinning, state->NextWaitingThreadId));
                    if (WaitBits.compare_exchange_weak(current, updated)) {
                        // We have successfully claimed this thread as spinning
                        Y_ABORT_UNLESS(state->Waiting);
                        state->Waiting = false;
                        state->Spinning = true;
                        state->WaitPad.Unpark();
                        return;
                    }
                    // Note: shutdown may remove the whole list
                }
                // We will leave FlagNeedSpinning when there are no threads in the list
            }

            // Lost the race, current updated, keep trying
        }
    }

    void TWorkStealingExecutorPool::Schedule(TInstant deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);

        Schedule(deadline - ActorSystem->Timestamp(), ev, cookie, workerId);
    }

    void TWorkStealingExecutorPool::Schedule(TMonotonic deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);

        const auto current = ActorSystem->Monotonic();
        if (deadline < current)
            deadline = current;

        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TWorkStealingExecutorPool::Schedule(TDuration delta, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_DEBUG_ABORT_UNLESS(workerId < PoolThreads);

        const auto deadline = ActorSystem->Monotonic() + delta;
        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TWorkStealingExecutorPool::Prepare(TActorSystem* actorSystem, NSchedulerQueue::TReader** scheduleReaders, ui32* scheduleSz) {
        TAffinityGuard affinityGuard(Affinity());

        ActorSystem = actorSystem;

        Threads.Reset(new TThreadCtx[PoolThreads]);
        ScheduleReaders.Reset(new NSchedulerQueue::TReader[PoolThreads]);
        ScheduleWriters.Reset(new NSchedulerQueue::TWriter[PoolThreads]);

        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread.Reset(
                new TExecutorThread(
                    i,
                    0,
                    actorSystem,
                    this,
                    MailboxTable.Get(),
                    PoolName,
                    Config.TimePerMailbox,
                    Config.EventsPerMailbox));
            ScheduleWriters[i].Init(ScheduleReaders[i]);
        }

        *scheduleReaders = ScheduleReaders.Get();
        *scheduleSz = PoolThreads;
    }

    void TWorkStealingExecutorPool::Start() {
        TAffinityGuard affinityGuard(Affinity());

        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread->Start();
        }
    }

    void TWorkStealingExecutorPool::PrepareStop() {
        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread->StopFlag = true;
        }

        ui64 current = WaitBits.load();
        for (;;) {
            ui64 headThreadId = GetWaitingThreadId(current);
            ui64 updated = ChangeWaitingThreadId(current, 0) | FlagNeedShutdown;
            if (WaitBits.compare_exchange_weak(current, updated)) {
                while (headThreadId) {
                    TThreadCtx* state = &Threads[headThreadId - 1];
                    headThreadId = state->NextWaitingThreadId;
                    Y_ABORT_UNLESS(state->Waiting);
                    state->Waiting = false;
                    state->WaitPad.Interrupt();
                }
                break;
            }
        }
    }

    void TWorkStealingExecutorPool::Shutdown() {
        for (i16 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread->Join();
        }
    }

    void TWorkStealingExecutorPool::GetCurrentStats(TExecutorPoolStats& poolStats, TVector<TExecutorThreadStats>& statsCopy) const {
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

    TString TWorkStealingExecutorPool::GetName() const {
        return PoolName;
    }

} // namespace NActors
