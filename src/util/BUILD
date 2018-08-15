cc_library(
    name = "log_lib",
    hdrs = ["log.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "error_lib",
    srcs = ["error.cc"],
    hdrs = ["error.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "program_writer_lib",
    srcs = ["program_writer.cc"],
    hdrs = ["program_writer.h"],
    deps = [
        ":log_lib",
        ":error_lib",
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

cc_library(
    name = "program_reader_lib",
    srcs = ["program_reader.cc"],
    hdrs = ["program_reader.h"],
    deps = [
        ":log_lib",
        ":error_lib",
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
