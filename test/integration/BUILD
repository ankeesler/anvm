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
        "//src/asm:assembler_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
    ],
    data = [
        "//test/integration/fixtures:fixtures",
    ],
)
