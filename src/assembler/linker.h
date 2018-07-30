#ifndef ANVM_ASSEMBLER_LINKER_H_
#define ANVM_ASSEMBLER_LINKER_H_

#include <ostream>
#include <vector>

#include "log.h"
#include "parser.h"
#include "src/cpu.h"
#include "src/program.h"

class Linker {
    public:
        Linker(Log *log) : log_(log) { }

        const char *Link(const Parser::Result& result, Program *program);

    private:
        const char *WriteFunction(const Parser::Function& function, Program *program);
        const char *WriteStatement(const Parser::Statement& statement, Program *program);
        const char *WriteLoadStatement(const Parser::Statement& statement, Program *program);
        const char *WriteStoreStatement(const Parser::Statement& statement, Program *program);

        Log *log_;
};

#endif // ANVM_ASSEMBLER_LINKER_H_
