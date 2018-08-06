#ifndef ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
#define ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_

#include <string>
#include <vector>
#include <map>

#include "src/cpu.h"
#include "src/util/log.h"

class SymbolTable {
    public:
        struct Function {
            bool operator==(const Function& f) const { return name == f.name && words == f.words; }

            std::string name;
            std::vector<Word> words;
        };

        SymbolTable(Log *log) : log_(log) { }
        ~SymbolTable();

        const std::vector<std::string>& FunctionNames() const;
        const std::vector<Function*>& FindFunctions(const std::string& name);
        Function *AddFunction(const Function& function);

    private:
        Log *log_;
        std::vector<std::string> names_;
        std::map<std::string, std::vector<Function*>> functions_;
};

#endif // ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
