# Generated by devtools/yamaker.

LIBRARY()

VERSION(15.0.5)

LICENSE(Apache-2.0 WITH LLVM-exception)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

SUBSCRIBER(g:cpp-contrib)

PEERDIR(
    contrib/libs/afl/llvm_mode
)

NO_COMPILER_WARNINGS()

NO_UTIL()

NO_SANITIZE()

SRCS(
    afl_driver.cpp
)

END()
