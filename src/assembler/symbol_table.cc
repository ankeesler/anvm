#include "src/assembler/symbol_table.h"

#include <vector>
#include <string>

#include "src/util/log.h"

#define LOG(...) log_->Printf("symbol_table", __FILE__, __LINE__, __VA_ARGS__)

SymbolTable::~SymbolTable() {
    for (std::map<std::string, std::vector<Symbol*>>::const_iterator it = symbols_.cbegin();
            it != symbols_.cend();
            ++it) {
        for (const Symbol *symbol : it->second) {
            delete symbol;
        }
    }
}

const std::vector<Symbol*>& SymbolTable::Symbols(const std::string& name) const {
    if (symbols_.count(name) == 0) {
        std::vector<Symbol*> empty;
        return std::move(empty);
    } else {
        return symbols_.at(name);
    }
}

Symbol *SymbolTable::AddSymbol(const Symbol& symbol) {
    const std::string& name = symbol.name;
    LOG("Adding symbol for name %s", name.c_str());

    Symbol *s = new Symbol(symbol);
    if (symbols_.count(name) == 0) {
        names_.push_back(name);
    }
    symbols_[name].push_back(s);

    return s;
}
