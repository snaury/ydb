# Generated by devtools/yamaker from nixpkgs 22.11.

LIBRARY()

LICENSE(
    BSD-2-Clause AND
    BSD-2-Clause-Views AND
    BSD-3-Clause AND
    MIT
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

VERSION(2024-09-24)

ORIGINAL_SOURCE(https://github.com/libcxxrt/libcxxrt/archive/40e4fa2049930412a2c43cdf0c39b6b5aa735341.tar.gz)

ADDINCL(
    contrib/libs/cxxsupp/libcxxrt
)

NO_COMPILER_WARNINGS()

NO_RUNTIME()

CXXFLAGS(-nostdinc++)

IF (CXX_UNWIND == "glibcxx_dynamic" OR ARCH_PPC64LE)
    LDFLAGS(-lgcc_s)
ELSE()
    PEERDIR(
        contrib/libs/libunwind
    )
ENDIF()

IF (SANITIZER_TYPE == undefined OR FUZZING)
    NO_SANITIZE()
    NO_SANITIZE_COVERAGE()
ENDIF()

SRCS(
    auxhelper.cc
    dynamic_cast.cc
    exception.cc
    guard.cc
    memory.cc
    stdexcept.cc
    typeinfo.cc
)

END()
