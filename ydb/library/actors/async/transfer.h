#pragma once
#include "async.h"
#include <ydb/library/actors/core/events.h>

namespace NActors {

    namespace NDetail {

        template<class TCallback>
        struct TAsyncActorTransferCallbackTraits;

        template<class T, class U>
        struct TAsyncActorTransferCallbackTraits<T U::*> {
            using TActorType = U;
        };

        template<class TCallback>
        concept IsActorTransferMemberFunction = (
            requires {
                typename TAsyncActorTransferCallbackTraits<TCallback>::TActorType;
            } &&
            std::is_base_of_v<IActor, typename TAsyncActorTransferCallbackTraits<TCallback>::TActorType>);

        template<class T, class TActorType, class TCallback, class... TArgs>
        class TAsyncActorTransferAwaiter {
            enum class EState : int {
                Pending = 0,
                Delivered = 1,
                Undelivered = 2,
                CancelPending = 3,
                CancelDelivered = 4,
                Finished = 5,
                Cancelled = 6,
                Destroyed = 7,
            };

        public:
            static constexpr bool IsActorAwareAwaiter = true;

            template<class TCallbackArg>
            TAsyncActorTransferAwaiter(const TActorId& actorId, TCallbackArg&& callback, TArgs&&... args)
                : TargetActorId(actorId)
                , Callback(std::forward<TCallbackArg>(callback))
                , CallbackArgs(std::forward<TArgs>(args)...)
            {}

            ~TAsyncActorTransferAwaiter() {
                State->MarkDestroyed();
            }

            TAsyncActorTransferAwaiter(const TAsyncActorTransferAwaiter&) = delete;
            TAsyncActorTransferAwaiter& operator=(const TAsyncActorTransferAwaiter&) = delete;

            bool await_ready() const noexcept { return false; }

            template<class TPromise>
            void await_suspend(std::coroutine_handle<TPromise> parent) {
                IActor& actor = parent.promise().GetActor();
                SelfActor = &actor;
                SelfId = actor.SelfId();
                Continuation = parent;
                actor.Send(TargetActorId, State->GetActionStart().release());
            }

            std::coroutine_handle<> await_cancel(std::coroutine_handle<> cancellation) {
                switch (State->Cancel()) {
                    case EState::Cancelled:
                        return cancellation;

                    case EState::CancelPending: {
                        // Notify the target actor about the cancellation
                        auto event = std::make_unique<TEvents::TEvResumeRunnable>(State->GetCancelAction());
                        State->Ref();
                        SelfActor->Send(TargetActorId, event.release());
                        Cancellation = cancellation;
                        return nullptr;
                    }

                    default:
                        // We must wait until target actor wakes us before processing cancellation
                        Cancellation = cancellation;
                        return nullptr;
                }
            }

        private:
            template<std::size_t... I>
            async<T> RunCallback(TActorType& self, std::index_sequence<I...>) {
                return std::invoke(Callback, self, std::get<I>(std::move(CallbackArgs))...);
            }

            void StartInTargetActor(IActor* actor) {
                Y_DEBUG_ABORT_UNLESS(dynamic_cast<TActorType*>(actor));
                TActorType& self = static_cast<TActorType&>(*actor);
                Coroutine.UnsafeMoveFrom(RunCallback(self,
                    std::make_index_sequence<std::tuple_size_v<std::tuple<TArgs&&...>>>()));
            }

            void CancelInTargetActor() {
                // TODO
            }

            bool HasResult() const {
                if (Coroutine) {
                    TAsyncResult<T>& result = Coroutine.GetHandle().promise();
                    return bool(result);
                } else {
                    return false;
                }
            }

            void Return() {
                // Running in the original actor
                if (!Cancellation || HasResult()) {
                    // We either haven't been cancelled, or the coroutine completed with the result
                    Continuation.resume();
                } else {
                    // Confirm cancellation, this includes when event was undelivered
                    Cancellation.resume();
                }
            }

        private:
            template<class TDerived>
            class TActionStart : public TActorRunnableItem::TImpl<TActionStart<TDerived>> {
                friend TActorRunnableItem::TImpl<TActionStart<TDerived>>;

                void DoRun(IActor* actor) noexcept {
                    static_cast<TDerived&>(*this).DoActionStart(actor);
                }
            };

            template<class TDerived>
            class TActionCancel : public TActorRunnableItem::TImpl<TActionCancel<TDerived>> {
                friend TActorRunnableItem::TImpl<TActionCancel<TDerived>>;

                void DoRun(IActor* actor) noexcept {
                    static_cast<TDerived&>(*this).DoActionCancel(actor);
                }
            };

            template<class TDerived>
            class TActionReturn : public TActorRunnableItem::TImpl<TActionReturn<TDerived>> {
                friend TActorRunnableItem::TImpl<TActionReturn<TDerived>>;

                void DoRun(IActor* actor) noexcept {
                    static_cast<TDerived&>(*this).DoActionReturn(actor);
                }
            };

            class TState
                : public TThrRefBase
                , private TActionStart<TState>
                , private TActionCancel<TState>
                , private TActionReturn<TState>
            {
                friend TActionStart<TState>;
                friend TActionCancel<TState>;
                friend TActionReturn<TState>;

            public:
                TState(TAsyncActorTransferAwaiter& self)
                    : Self(self)
                {}

                auto GetActionStart() {
                    TActionStart<TState>* action = this;
                    auto event = std::make_unique<TEvents::TEvResumeRunnable>(action);
                    Ref();
                    return event;
                }

                auto GetActionCancel() {
                    TActionCancel<TState>* action = this;
                    auto event = std::make_unique<TEvents::TEvResumeRunnable>(action);
                    Ref();
                    return event;
                }

                auto GetActionReturn() {
                    TActionReturn<TState>* action = this;
                    auto event = std::make_unique<TEvents::TEvResumeRunnable>(action);
                    Ref();
                    return event;
                }

                EState Cancel() {
                    EState state = State.load(std::memory_order_relaxed);
                    for (;;) {
                        switch (state) {
                            case EState::Pending:
                                if (!State.compare_exchange_weak(state, EState::Cancelled, std::memory_order_relaxed)) {
                                    continue; // Outdated state, or state change race
                                }
                                return EState::Cancelled;

                            case EState::Delivered:
                                if (!State.compare_exchange_weak(state, EState::CancelPending, std::memory_order_relaxed)) {
                                    continue; // Outdated state, or state change race
                                }
                                return EState::CancelPending;

                            default:
                                // In other states cancellation is pointless
                                // The coroutine might have finished already and we need to wait
                                return state;
                        }
                    }
                }

                void MarkDestroyed() {
                    State.store(EState::Destroyed, std::memory_order_relaxed);
                }

            private:
                void DoActionStart(IActor* actor) {
                    Y_DEFER { UnRef(); };
                    EState state = State.load(std::memory_order_relaxed);
                    for (;;) {
                        switch (state) {
                            case EState::Pending:
                                if (actor) [[likely]] {
                                    // We are running inside the target actor id
                                    if (!State.compare_exchange_weak(state, EState::Delivered, std::memory_order_relaxed)) {
                                        continue; // Outdated state, or state change race
                                    }
                                    // We successfully moved to the delivered state, start the coroutine
                                    Self.StartInTargetActor(actor);
                                } else {
                                    // Message could not be delivered, e.g. actor does not exist
                                    if (!State.compare_exchange_weak(state, EState::Undelivered, std::memory_order_relaxed)) {
                                        continue; // Outdated state, or state change race
                                    }
                                    // We must resume the parent actor
                                    if (TlsActivationContext) {
                                        TlsActivationContext->Send(new IEventHandle(
                                            Self.SelfId, TActorId(), GetActionReturn().release()));
                                    }
                                }
                                return;

                            case EState::Cancelled:
                                // The coroutine was cancelled before the start action could be delivered
                                return;

                            case EState::Destroyed:
                                // The parent frame was destroyed before the start action could be delivered
                                return;

                            default:
                                Y_DEBUG_ABORT_UNLESS(false, "Unexpected state when processing the start action");
                                return;
                        }
                    }
                }

                void DoActionCancel(IActor* actor) {
                    Y_DEFER { UnRef(); };
                    EState state = State.load(std::memory_order_relaxed);
                    for (;;) {
                        switch (state) {
                            case EState::CancelPending:
                                if (actor) [[likely]] {
                                    if (!State.compare_exchange_weak(state, EState::CancelDelivered, std::memory_order_relaxed)) {
                                        continue; // Outdated state, or state change race
                                    }
                                    // We successfully confirmed cancellation as delivered
                                    Self.CancelInTargetActor();
                                }
                                return;

                            case EState::Finished:
                                // Coroutine finished before the cancel action could be delivered
                                return;

                            case EState::Destroyed:
                                // The parent frame was destroyed before the cancel action could be delivered
                                return;

                            default:
                                Y_DEBUG_ABORT_UNLESS(false, "Unexpected state when processing the cancel action");
                                return;
                        }
                    }
                }

                void DoActionReturn(IActor* actor) {
                    Y_DEFER { UnRef(); };
                    EState state = State.load(std::memory_order_relaxed);
                    switch (state) {
                        case EState::Finished:
                            // Coroutine has not been destroyed yet
                            if (actor) [[likely]] {
                                Self.Return();
                            }
                            return;

                        case EState::Destroyed:
                            // Coroutine has already been destroyed
                            return;

                        default:
                            Y_DEBUG_ABORT_UNLESS(false, "Unexpected state when processing the return action");
                            return;
                    }
                }

            private:
                TAsyncActorTransferAwaiter& Self;
                std::atomic<EState> State{ EState::Pending };
            };

        private:
            const TActorId TargetActorId;
            IActor* SelfActor;
            TActorId SelfId;
            Y_NO_UNIQUE_ADDRESS TCallback Callback;
            Y_NO_UNIQUE_ADDRESS std::tuple<TArgs&&...> CallbackArgs;
            TIntrusivePtr<TState> State = MakeIntrusive<TState>(*this);
            std::coroutine_handle<> Continuation;
            std::coroutine_handle<> Cancellation;
            async<T> Coroutine = async<T>::UnsafeEmpty();
        };

    } // namespace NDetail

    // template<class TActorType, class TCallback, class... TArgs>
    // auto AsyncActorCall(const TActorId& actorId, TCallback&& callback, TArgs&&... args) {
    //     return NDetail::TAsyncActorTransferAwaiter<TActorType, TCallback&&, TArgs...>(
    //         actorId, std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
    // }

    template<NDetail::IsActorTransferMemberFunction TCallback, class... TArgs>
    auto AsyncActorCall(const TActorId& actorId, TCallback callback, TArgs&&... args) {
        using TActorType = typename NDetail::TAsyncActorTransferCallbackTraits<TCallback>::TActorType;
        using TInvokeResult = std::invoke_result_t<TCallback, TActorType&, TArgs&&...>;
        static_assert(IsAsyncCoroutine<TInvokeResult>, "Member function must return an async<T>");
        using T = typename TInvokeResult::result_type;
        return NDetail::TAsyncActorTransferAwaiter<T, TActorType, TCallback, TArgs...>(
            actorId, std::move(callback), std::forward<TArgs>(args)...);
    }

} // namespace NActors
