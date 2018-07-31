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
        "//src/util:program_writer_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
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
        "//src/util:program_reader_lib",
        "//src/util:log_lib",
    ],
)

cc_test(
    name = "error_test",
    srcs = ["error_test.cc"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        "//src/util:error_lib",
    ],
)
