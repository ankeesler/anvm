cc_library(
    name = "parser_lib",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        "//src/util:log_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "linker_lib",
    srcs = ["linker.cc"],
    hdrs = ["linker.h"],
    deps = [
        ":symbol_table_lib",
        "//src:core_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "symbol_table_lib",
    srcs = ["symbol_table.cc"],
    hdrs = ["symbol_table.h"],
    deps = [
        "//src/util:log_lib",
        "//src:core_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "symbol_table_populator_lib",
    srcs = ["symbol_table_populator.cc"],
    hdrs = ["symbol_table_populator.h"],
    deps = [
        ":parser_lib",
        ":symbol_table_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
        "//src:core_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "assembler_lib",
    srcs = ["assembler.cc"],
    hdrs = ["assembler.h"],
    deps = [
        ":parser_lib",
        ":symbol_table_populator_lib",
        ":symbol_table_lib",
        ":linker_lib",
        "//src/util:program_writer_lib",
        "//src/util:error_lib",
        "//src/util:log_lib",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "anasm",
    srcs = ["main.cc"],
    deps = [
        ":assembler_lib",
        "//src/util:log_lib",
        "//src/util:error_lib",
    ],
)
