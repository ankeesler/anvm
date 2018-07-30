#ifndef ANVM_ASSEMBLER_LINKER_H_
#define ANVM_ASSEMBLER_LINKER_H_

#include <ostream>
#include <vector>

#include "log.h"
#include "parser.h"
#include "src/cpu.h"

class Linker {
    public:
        Linker(Log *log) : log_(log) { }

        const char *Link(const Parser::Result& result, std::vector<Word> *output);

    private:
        const char *WriteFunction(const Parser::Function& function, std::vector<Word> *output); 
        const char *WriteStatement(const Parser::Statement& statement, std::vector<Word> *output);
        const char *WriteLoadStatement(const Parser::Statement& statement, std::vector<Word> *output);
        const char *WriteStoreStatement(const Parser::Statement& statement, std::vector<Word> *output);

        Log *log_;
};

#endif // ANVM_ASSEMBLER_LINKER_H_
