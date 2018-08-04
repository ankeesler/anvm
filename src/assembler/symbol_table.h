#ifndef ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
#define ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_

#include <string>
#include <vector>
#include <map>

#include "src/cpu.h"
#include "src/util/log.h"

class SymbolTable {
    public:
        class Function {
            public:
                bool operator==(const Function& f) const { return name_ == f.name_ && words_ == f.words_; }

                const std::string& Name() const { return name_; }
                void SetName(const std::string& name) { name_ = name; }

                const std::vector<Word> Words() const { return words_; }
                void AddWord(Word w) { words_.push_back(w); }

            private:
                std::string name_;
                std::vector<Word> words_;
        };

        SymbolTable(Log *log) : log_(log) { }

        const std::vector<std::string>& FunctionNames() const;
        const std::vector<Function>& FindFunctions(const std::string& name);
        void AddFunction(const Function& function);

    private:
        Log *log_;
        std::vector<std::string> names_;
        std::map<std::string, std::vector<Function>> functions_;
};

#endif // ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
