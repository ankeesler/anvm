#include <string.h>
#include <iostream>
#include <fstream>

#include "src/program.h"
#include "src/system.h"
#include "src/util/program_reader.h"
#include "src/util/log.h"

static bool ParseFlags(int argc, char *argv[]);
static void PrintUsage();

static bool ParseRunFlags(int i, int argc, char *argv[]);
static int RunAction();

static Log *log = nullptr;
static bool debug = false;
static const char *command = nullptr;

static const char *runInFile = nullptr;

int main(int argc, char *argv[]) {
    if (!ParseFlags(argc, argv)) {
        PrintUsage();
        return 1;
    }

    if (debug) {
        log = new StdoutLog();
    } else {
        log = new VoidLog();
    }

    if (strcmp(command, "run") == 0) {
        return RunAction();
    }

    return 0;
}

static bool ParseFlags(int argc, char *argv[]) {
    int i = 1;
    for (; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if (command == nullptr) {
            command = argv[i];
            i++;
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

static int RunAction() {
    Program p;
    ProgramReader pr(log);
    std::ifstream ifs(runInFile);
    Error error = pr.Read(ifs, &p);
    if (error) {
        std::cout << "ERROR: " << error.S() << std::endl;
        return 1;
    }

    CPU cpu(8, 256);
    System s(&cpu);
    Word status = s.Run(p);
    if (status != 0) {
        std::cout << "ERROR: System::Run returned " << status << std::endl;
        return 1;
    }

    return 0;
}
