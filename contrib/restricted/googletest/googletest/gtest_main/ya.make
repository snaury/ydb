# Generated by devtools/yamaker.

LIBRARY()

LICENSE(BSD-3-Clause)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

SUBSCRIBER(g:cpp-contrib)

VERSION(1.15.2)

PEERDIR(
    contrib/restricted/googletest/googletest
)

ADDINCL(
    contrib/restricted/googletest/googletest
    contrib/restricted/googletest/googletest/include
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCDIR(contrib/restricted/googletest/googletest/src)

SRCS(
    GLOBAL gtest_main.cc
)

END()
