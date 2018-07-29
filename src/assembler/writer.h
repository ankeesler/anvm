#ifndef ANVM_ASSEMBLER_WRITER_H_
#define ANVM_ASSEMBLER_WRITER_H_

#include <ostream>

#include "log.h"
#include "parser.h"

class Writer {
    public:
        Writer(Log *log) : log_(log) { }

        const char *Write(const Parser::Result& result, std::ostream& os);

    private:
        Log *log_;
};

#endif // ANVM_ASSEMBLER_WRITER_H_
