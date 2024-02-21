UNITTEST_FOR(ydb/core/tx/datashard)

PEERDIR(
    ydb/core/tx/datashard/ut_common
    ydb/core/kqp/ut/common
    ydb/core/testlib/default
    yql/essentials/public/udf/service/exception_policy
    ydb/public/lib/yson_value
    ydb/public/sdk/cpp/src/client/result
)

YQL_LAST_ABI_VERSION()

SRCS(
    datashard_ut_debug_info.cpp
)

REQUIREMENTS(ram:32)

END()
