#ifndef ANVM_UTIL_PROGRAM_READER_H_
#define ANVM_UTIL_PROGRAM_READER_H_

#include <istream>

#include "log.h"
#include "error.h"
#include "src/program.h"

class ProgramReader {
    public:
        ProgramReader(Log *log) : log_(log) { }

        const Error Read(std::istream& is, Program *program);

    private:
        Log *log_;
};

#endif // ANVM_UTIL_PROGRAM_READER_H_
