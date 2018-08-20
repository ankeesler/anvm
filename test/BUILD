cc_test(
    name = "core_test",
    srcs = [
        "cpu_test.cc",
        "program_test.cc",
        "process_test.cc",
    ],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/include",
        "-Wall",
        "-Werror",
        "-g",
        "-ggdb",
        "-O0",
    ],
    deps = [
        "@gtest//:gtest",
        "@gtest//:gmock",
        "@gtest//:gmock_main",
        ":program_library_lib",
        "//src:core_lib",
    ],
)

cc_library(
    name = "program_library_lib",
    hdrs = ["program_library.h"],
    deps = [
        "//src:core_lib",
    ],
    copts = [
        "-Wall",
        "-Werror",
        "-g",
        "-ggdb",
        "-O0",
    ],
    visibility = ["//visibility:public"],
)
