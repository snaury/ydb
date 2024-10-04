# Generated by devtools/yamaker.

PROGRAM()

LICENSE(Apache-2.0)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

INDUCED_DEPS(
    h+cpp
    ${ARCADIA_ROOT}/contrib/libs/flatbuffers/include/flatbuffers/flatbuffers.h
    ${ARCADIA_ROOT}/contrib/libs/flatbuffers/include/flatbuffers/flatbuffers_iter.h
)

VERSION(24.3.25)

PEERDIR(
    contrib/restricted/abseil-cpp/absl/base
)

ADDINCL(
    contrib/libs/flatbuffers/grpc
    contrib/libs/flatbuffers/include
)

NO_COMPILER_WARNINGS()

NO_UTIL()

CFLAGS(
    -DFLATBUFFERS_LOCALE_INDEPENDENT=1
)

SRCDIR(contrib/libs/flatbuffers)

SRCS(
    grpc/src/compiler/cpp_generator.cc
    grpc/src/compiler/go_generator.cc
    grpc/src/compiler/java_generator.cc
    grpc/src/compiler/python_generator.cc
    grpc/src/compiler/swift_generator.cc
    grpc/src/compiler/ts_generator.cc
    src/annotated_binary_text_gen.cpp
    src/bfbs_gen_lua.cpp
    src/bfbs_gen_nim.cpp
    src/binary_annotator.cpp
    src/code_generators.cpp
    src/file_binary_writer.cpp
    src/file_name_saving_file_manager.cpp
    src/file_writer.cpp
    src/flatc.cpp
    src/flatc_main.cpp
    src/idl_gen_binary.cpp
    src/idl_gen_cpp.cpp
    src/idl_gen_cpp_yandex_maps_iter.cpp
    src/idl_gen_csharp.cpp
    src/idl_gen_dart.cpp
    src/idl_gen_fbs.cpp
    src/idl_gen_go.cpp
    src/idl_gen_grpc.cpp
    src/idl_gen_java.cpp
    src/idl_gen_json_schema.cpp
    src/idl_gen_kotlin.cpp
    src/idl_gen_kotlin_kmp.cpp
    src/idl_gen_lobster.cpp
    src/idl_gen_php.cpp
    src/idl_gen_python.cpp
    src/idl_gen_rust.cpp
    src/idl_gen_swift.cpp
    src/idl_gen_text.cpp
    src/idl_gen_ts.cpp
    src/idl_parser.cpp
    src/reflection.cpp
    src/util.cpp
)

END()
