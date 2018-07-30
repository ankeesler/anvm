cc_library(
    name = "gtest",
    srcs = glob(
        ["googletest/src/*.cc"],
        exclude = [
            "googletest/src/gtest-all.cc",
            "googletest/src/gtest_main.cc",
        ]
    ),
    hdrs = glob([
        "googletest/include/**/*.h",
        "googletest/src/*.h"
    ]),
    copts = [
        "-Iexternal/gtest/googletest/",
        "-Iexternal/gtest/googletest/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gtest_main",
    srcs = ["googletest/src/gtest_main.cc"],
    hdrs = glob([
        "googletest/include/**/*.h",
        "googletest/src/*.h"
    ]),
    copts = [
        "-Iexternal/gtest/googletest/",
        "-Iexternal/gtest/googletest/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gmock",
    srcs = glob(
        ["googlemock/src/*.cc"],
        exclude = [
            "googlemock/src/gmock-all.cc",
            "googlemock/src/gmock_main.cc",
        ]
    ),
    hdrs = glob([
        "googlemock/include/**/*.h",
        "googlemock/src/*.h"
    ]),
    deps = ["@gtest//:gtest"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/",
        "-Iexternal/gtest/googlemock/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gmock_main",
    srcs = ["googlemock/src/gmock_main.cc"],
    hdrs = glob([
        "googlemock/include/**/*.h",
        "googlemock/src/*.h"
    ]),
    deps = ["@gtest//:gtest"],
    copts = [
        "-Iexternal/gtest/googletest/include",
        "-Iexternal/gtest/googlemock/",
        "-Iexternal/gtest/googlemock/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
