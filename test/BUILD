cc_test(
    name = "core_test",
    srcs = [
        "cpu_test.cc",
        "program_test.cc",
    ],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src:core_lib",
    ],
)
