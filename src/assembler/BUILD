cc_library(
    name = "parser_lib",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        "//src/util:log_lib",
        "//src:core_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "linker_lib",
    srcs = ["linker.cc"],
    hdrs = ["linker.h"],
    deps = [
        "//src/util:log_lib",
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
        "//src/util:program_writer_lib",
        "//src/util:log_lib",
    ],
)
