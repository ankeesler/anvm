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
        "@gtest//:main",
        "//src/assembler:parser_lib",
        ":log_lib",
    ],
)

cc_test(
    name = "writer_test",
    srcs = ["writer_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        "@gtest//:main",
        "//src/assembler:parser_lib",
        "//src/assembler:writer_lib",
        ":log_lib",
    ],
)
