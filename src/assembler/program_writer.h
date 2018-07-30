#ifndef ANVM_ASSEMBLER_PROGRAM_WRITER_H_
#define ANVM_ASSEMBLER_PROGRAM_WRITER_H_

#include <ostream>

#include "log.h"
#include "src/program.h"

class ProgramWriter {
    public:
        ProgramWriter(Log *log) : log_(log) { }

        const char *Write(const Program& program, std::ostream& os);

    private:
        Log *log_;
};

#endif // ANVM_ASSEMBLER_PROGRAM_WRITER_H_
