cc_library(
    name = "log_lib",
    hdrs = ["log.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "parser_lib",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        ":log_lib",
        "//src:core_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "linker_lib",
    srcs = ["linker.cc"],
    hdrs = ["linker.h"],
    deps = [
        ":log_lib",
        ":parser_lib",
        "//src:core_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "anasm",
    srcs = ["main.cc"],
    deps = [
        ":parser_lib",
        ":linker_lib",
    ],
)
