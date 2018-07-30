cc_test(
    name = "parser_test",
    srcs = ["parser_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gtest_main",
        "//src/assembler:parser_lib",
        "//src/util:log_lib",
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
        "//src/util:log_lib",
    ],
)
