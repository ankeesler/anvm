cc_binary(
    name = "anvm",
    srcs = ["main.cc"],
    deps = [
        "//src:core_lib",
        "//src/util:log_lib",
        "//src/util:program_reader_lib",
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
