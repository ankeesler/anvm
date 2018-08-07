#ifndef ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
#define ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_

#include <string>
#include <vector>
#include <map>

#include "src/cpu.h"
#include "src/util/log.h"

struct Symbol {
    enum Type {
        FUNCTION,
        VARIABLE,
    };

    Symbol(const std::string& name, bool resolved, enum Type type)
        : name(name), resolved(resolved), type(type) { }

    bool operator==(const Symbol& s) const {
        return (name == s.name
                && resolved == s.resolved
                && type == s.type
                && words == s.words);
    }

    std::string name;
    bool resolved;
    enum Type type;
    std::vector<Word> words;
};

class SymbolTable {
    public:
        SymbolTable(Log *log) : log_(log) { }
        ~SymbolTable();

        const std::vector<std::string>& SymbolNames() const { return names_; }
        const std::vector<Symbol*>& Symbols(const std::string& name);

        Symbol *AddSymbol(const Symbol& symbol);

    private:
        Log *log_;
        std::vector<std::string> names_;
        std::map<std::string, std::vector<Symbol*>> symbols_;
};

#endif // ANVM_SRC_ASSEMBLER_SYMBOL_TABLE_H_
