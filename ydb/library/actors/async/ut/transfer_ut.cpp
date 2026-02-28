#include <ydb/library/actors/async/transfer.h>

#include "common.h"

namespace NAsyncTest {

    class TMyClass : public IActor {
    public:
        async<void> MyMethod() {
            co_return;
        }
    };

    Y_UNIT_TEST_SUITE(Transfer) {

        Y_UNIT_TEST(Basics) {
            AsyncActorCall(TActorId(), &TMyClass::MyMethod);
        }

    }

} // namespace NAsyncTest
