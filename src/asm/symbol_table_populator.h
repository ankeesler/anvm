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
            : log_(log), st_(st) { }

        const std::vector<Error>& Errors() const { return state_.errors; }

        void OnStart();
        void OnError(const std::string& s, int line_num);
        void OnFunction(const std::string& name, int line_num);
        void OnInstruction(const std::string& name, int line_num);
        void OnArg(enum ArgType type, const std::string& name, int line_num);
        void OnEnd();

    private:
        struct State {
            std::vector<Error> errors;
            int line_num;

            Symbol *function;

            Word instruction;
            int expected_args;
            int actual_args;
        };

        void ResetState();
        bool GetLoadInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);
        bool GetStoreInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);
        bool GetBranchInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w);

        Log *log_;
        SymbolTable *st_;
        State state_;
};

#endif // ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_POPULATOR_H_
