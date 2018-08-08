#ifndef ANVM_ASSEMBLER_LINKER_H_
#define ANVM_ASSEMBLER_LINKER_H_

#include <ostream>
#include <vector>

#include "symbol_table.h"
#include "src/program.h"
#include "src/util/log.h"
#include "src/util/error.h"

class Linker {
    public:
        Linker(Log *log) : log_(log) { }

        Error Link(const SymbolTable& st, Program *program);

    private:
        Log *log_;
};

#endif // ANVM_ASSEMBLER_LINKER_H_
