#ifndef ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_POPULATOR_H_
#define ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_POPULATOR_H_

#include <string>

#include "parser.h"
#include "symbol_table.h"
#include "src/util/log.h"
#include "src/util/error.h"

class SymbolTablePopulator : public Parser::Handler {
    public:
        SymbolTablePopulator(Log *log, SymbolTable *st)
            : log_(log), st_(st), currentFunction_(nullptr) { }

        const std::vector<Error>& Errors() const { return errors_; }

        void OnStart();
        void OnError(const std::string& s, int line_num);
        void OnFunction(const std::string& name, int line_num);
        void OnInstruction(const std::string& name, int line_num);
        void OnArg(enum ArgType type, const std::string& name, int line_num);
        void OnEnd();

    private:
        bool GetLoadInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);
        bool GetStoreInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);
        bool GetBranchInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);

        Log *log_;
        SymbolTable *st_;
        Symbol *currentFunction_;
        std::vector<Error> errors_;
};

#endif // ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_POPULATOR_H_
