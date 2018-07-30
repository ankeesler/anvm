cc_library(
    name = "log_lib",
    hdrs = ["log.h"],
    deps = ["//src/assembler:log_lib"],
    visibility = ["//visibility:public"],
)
    
cc_test(
    name = "parser_test",
    srcs = ["parser_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gtest_main",
        "//src/assembler:parser_lib",
        ":log_lib",
    ],
)

cc_test(
    name = "linker_test",
    srcs = ["linker_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/assembler:parser_lib",
        "//src/assembler:linker_lib",
        ":log_lib",
    ],
)

cc_test(
    name = "program_writer_test",
    srcs = ["program_writer_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/assembler:program_writer_lib",
        ":log_lib",
    ],
)

cc_test(
    name = "program_reader_test",
    srcs = ["program_reader_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/assembler:program_reader_lib",
        ":log_lib",
    ],
)
