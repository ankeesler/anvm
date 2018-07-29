gtest_env = Environment(
    CXXFLAGS = "--std=c++11 -g -Wall -Werror -O0 -I../googletest/googletest -I../googletest/googletest/include",
)
gtest_all_obj = gtest_env.Object(
    target = "third_party/gtest/gtest-all.o",
    source = "../googletest/googletest/src/gtest-all.cc",
)
gtest_main_obj = gtest_env.Object(
    target = "third_party/gtest/gtest_main.o",
    source = "../googletest/googletest/src/gtest_main.cc",
)
gtest_library = Library(
    target = "gtest",
    source = [
        gtest_all_obj, 
        gtest_main_obj,
    ],
)

anvm_env = Environment(
    CXXFLAGS = "--std=c++11 -g -Wall -Werror -O0",
)
anvm_library = anvm_env.Library(
    target = "anvm",
    source = [
        "src/cpu.cc",
        "src/program.cc",
        "src/system.cc",
    ],
)

test_env = Environment(
    CXXFLAGS = "--std=c++11 -g -Wall -Werror -O0 -I. -I../googletest/googletest/include",
)
test_executable = test_env.Program(
    target = "tests",
    source = [
        anvm_library,
        gtest_library,
        "test/test.cc",
    ],
)

parser_test_executable = test_env.Program(
    target = "parser_tests",
    source = [
        gtest_library,
        "src/assembler/parser.cc",
        "test/assembler/parser_test.cc",
    ],
)

assembler_env = Environment(
    # This is dumb that we include googletest...we should just switch to CMake...
    CXXFLAGS = "--std=c++11 -g -Wall -Werror -O0 -I. -I../googletest/googletest/include",
)
assembler_executable = assembler_env.Program(
    target = "anasm",
    source = [
        "src/assembler/main.cc",
        "src/assembler/parser.cc",
    ],
)

