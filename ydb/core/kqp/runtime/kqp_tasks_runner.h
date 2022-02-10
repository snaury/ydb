#pragma once

#include "kqp_compute.h"
#include <ydb/library/yql/dq/runtime/dq_tasks_runner.h>

namespace NKikimr {
namespace NKqp {

NYql::NDq::IDqOutputConsumer::TPtr KqpBuildOutputConsumer(const NYql::NDqProto::TTaskOutput& outputDesc,
    const NMiniKQL::TType* type, NUdf::IApplyContext* applyCtx, const NMiniKQL::TTypeEnvironment& typeEnv,
    TVector<NYql::NDq::IDqOutput::TPtr>&& outputs);

TIntrusivePtr<NYql::NDq::IDqTaskRunner> CreateKqpTaskRunner(const NYql::NDq::TDqTaskRunnerContext& execCtx,
    const NYql::NDq::TDqTaskRunnerSettings& settings, const NYql::NDq::TLogFunc& logFunc);


class TKqpTasksRunner : public TSimpleRefCount<TKqpTasksRunner>, private TNonCopyable {
public:
    TKqpTasksRunner(const google::protobuf::RepeatedPtrField<NYql::NDqProto::TDqTask>& tasks,
                    const NYql::NDq::TDqTaskRunnerContext& execCtx, const NYql::NDq::TDqTaskRunnerSettings& settings,
                    const NYql::NDq::TLogFunc& logFunc);

    ~TKqpTasksRunner();

    void Prepare(const NYql::NDq::TDqTaskRunnerMemoryLimits& memoryLimits,
                 const NYql::NDq::IDqTaskRunnerExecutionContext& execCtx);

    NYql::NDq::ERunStatus Run(bool applyEffects);

    /// @returns (data_was_transferred, finished)
    std::pair<bool, bool> TransferData(ui64 fromTask, ui64 fromChannelId, ui64 toTask, ui64 toChannelId);

    NYql::NDq::IDqTaskRunner& GetTaskRunner(ui64 taskId);
    const NYql::NDq::IDqTaskRunner& GetTaskRunner(ui64 taskId) const;

    const NYql::NDqProto::TDqTask& GetTask(ui64 taskId) const;

    NYql::NDq::IDqInputChannel::TPtr GetInputChannel(ui64 taskId, ui64 channelId) {
        return GetTaskRunner(taskId).GetInputChannel(channelId);
    }
    NYql::NDq::IDqOutputChannel::TPtr GetOutputChannel(ui64 taskId, ui64 channelId) {
        return GetTaskRunner(taskId).GetOutputChannel(channelId);
    }

    // if memoryLimit = Nothing()  then don't set memory limit, use existing one (if any)
    // if memoryLimit = 0          then set unlimited
    // otherwise use particular memory limit
    TGuard<NMiniKQL::TScopedAlloc> BindAllocator(TMaybe<ui64> memoryLimit = Nothing());

    const TMap<ui64, const NYql::NDq::TDqTaskRunnerStats*> GetTasksStats() const { return Stats; };

private:
    TMap<ui64, TIntrusivePtr<NYql::NDq::IDqTaskRunner>> TaskRunners;
    TMap<ui64, const NYql::NDqProto::TDqTask*> Tasks;
    TMap<ui64, const NYql::NDq::TDqTaskRunnerStats*> Stats;
    NYql::NDq::TLogFunc LogFunc;
    NMiniKQL::TScopedAlloc* Alloc;
    NMiniKQL::TKqpComputeContextBase* ComputeCtx;

    enum EState {
        Initial = 0,
        Prepared = 1,
        Running = 2
    };
    EState State = EState::Initial;
};


TIntrusivePtr<TKqpTasksRunner> CreateKqpTasksRunner(const google::protobuf::RepeatedPtrField<NYql::NDqProto::TDqTask>& tasks,
    const NYql::NDq::TDqTaskRunnerContext& execCtx, const NYql::NDq::TDqTaskRunnerSettings& settings,
    const NYql::NDq::TLogFunc& logFunc);

} // namespace NKqp
} // namespace NKikimr
