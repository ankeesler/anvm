#include <string.h>
#include <iostream>
#include <fstream>

#include "assembler.h"
#include "src/util/log.h"

static bool ParseFlags(int argc, char *argv[]);
static void PrintUsage();

static const char *inFilename = nullptr;
static bool debug = false;
static const char *outFilename = "a.out";

int main(int argc, char *argv[]) {
    if (!ParseFlags(argc, argv)) {
        PrintUsage();
        return 1;
    }

    Log *log = nullptr;
    if (debug) {
        log = new StdoutLog();
    } else {
        log = new VoidLog();
    }

    std::ifstream ifs(inFilename);
    std::ofstream ofs(outFilename);

    Assembler a;
    Error error = a.Run(log, ifs, ofs);
    if (error) {
        std::cout << error << std::endl;
        return 1;
    }

    delete log;

    return 0;
}

static bool ParseFlags(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            i++;
            outFilename = argv[i];
        } else if (inFilename == nullptr) {
            inFilename = argv[i];
        } else {
            std::cout << "Unknown argument: " << argv[i] << std::endl;
            return false;
        }
    }

    return inFilename != nullptr;
}

static void PrintUsage() {
    std::cout << "Usage: anasm [-d|--debug] [-o|--output] file.asm" << std::endl;
    std::cout << "Required arguments:" << std::endl;
    std::cout << "    file.asm      Source assembly file" << std::endl;
    std::cout << "Optional arguments:" << std::endl;
    std::cout << "    -d|--debug    Enable debug printing to stdout" << std::endl;
    std::cout << "    -o|--output   Direct the assembled bytecode to a file" << std::endl;
}
