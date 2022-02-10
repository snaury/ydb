# Generated by devtools/yamaker.

LIBRARY()

OWNER(
    orivej
    g:cpp-contrib
)

LICENSE(Apache-2.0 WITH LLVM-exception)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm12 
    contrib/libs/llvm12/include 
    contrib/libs/llvm12/lib/Bitstream/Reader 
    contrib/libs/llvm12/lib/IR 
    contrib/libs/llvm12/lib/Support 
)

ADDINCL(
    contrib/libs/llvm12/lib/Bitcode/Reader
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    BitReader.cpp
    BitcodeAnalyzer.cpp
    BitcodeReader.cpp
    MetadataLoader.cpp
    ValueList.cpp
)

END()
