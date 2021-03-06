#ifndef ANVM_UTIL_PROGRAM_WRITER_H_
#define ANVM_UTIL_PROGRAM_WRITER_H_

#include <ostream>

#include "log.h"
#include "error.h"
#include "src/program.h"

class ProgramWriter {
    public:
        ProgramWriter(Log *log) : log_(log) { }

        Error Write(const Program& program, std::ostream& os);

    private:
        Log *log_;
};

#endif // ANVM_UTIL_PROGRAM_WRITER_H_
