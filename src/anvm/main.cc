#include <string.h>
#include <iostream>

static bool ParseFlags(int argc, char *argv[]);
static void PrintUsage();

static bool ParseRunFlags(int i, int argc, char *argv[]);

static const char *command = nullptr;

static const char *runInFile = nullptr;

int main(int argc, char *argv[]) {
    if (!ParseFlags(argc, argv)) {
        PrintUsage();
        return 1;
    }

    if (strcmp(command, "run") == 0) {
    }

    return 0;
}

static bool ParseFlags(int argc, char *argv[]) {
    int i = 1;
    for (; i < argc; i++) {
        if (command == nullptr) {
            command = argv[i];
            break;
        }
    }

    if (command != nullptr) {
        if (strcmp(command, "run") == 0) {
            if (!ParseRunFlags(i, argc, argv)) {
                return false;
            }
        }
    }

    return command != nullptr;
}

static void PrintUsage() {
    std::cout << "Usage: anvm <command>" << std::endl;
    std::cout << "Optional arguments:" << std::endl;
    std::cout << "    -d|--debug    Enable debug printing to stdout" << std::endl;
    std::cout << "    -o|--output   Direct the assembled bytecode to a file" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "    run <exe>     Run the provided <exe> file as a program in anvm" << std::endl;
}

static bool ParseRunFlags(int i, int argc, char *argv[]) {
    for (; i < argc; i++) {
        if (runInFile == nullptr) {
            runInFile = argv[i];
        }
    }

    return runInFile != nullptr;
}
