cc_test(
    name = "parser_test",
    srcs = ["parser_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
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
        "//src/assembler:linker_lib",
        "//src/assembler:symbol_table_lib",
        "//src:core_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
    ],
)

cc_test(
    name = "symbol_table_test",
    srcs = ["symbol_table_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/assembler:symbol_table_lib",
        "//src/util:log_lib",
    ],
)

cc_test(
    name = "symbol_table_populator_test",
    srcs = ["symbol_table_populator_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/assembler:symbol_table_populator_lib",
        "//src/assembler:symbol_table_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
    ],
)
