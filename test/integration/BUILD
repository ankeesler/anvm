cc_test(
    name = "assembler_test",
    srcs = ["assembler_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src:core_lib",
        "//src/asm:assembler_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
        "//src/util:program_reader_lib",
    ],
    data = [
        "//test/integration/fixtures:fixtures",
    ],
)
