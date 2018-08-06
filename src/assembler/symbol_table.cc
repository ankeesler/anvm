#include "src/assembler/symbol_table.h"

#include <vector>
#include <string>

#include "src/util/log.h"

#define LOG(...) log_->Printf("symbol_table", __FILE__, __LINE__, __VA_ARGS__)

SymbolTable::~SymbolTable() {
    for (std::map<std::string, std::vector<Function*>>::const_iterator it = functions_.cbegin();
            it != functions_.cend();
            ++it) {
        for (const Function *function : it->second) {
            delete function;
        }
    }
}

const std::vector<std::string>& SymbolTable::FunctionNames() const {
    return names_;
}

const std::vector<SymbolTable::Function*>& SymbolTable::FindFunctions(const std::string& name) {
    if (functions_.count(name) == 0) {
        std::vector<SymbolTable::Function*> empty;
        return std::move(empty);
    } else {
        return functions_[name];
    }
}

SymbolTable::Function *SymbolTable::AddFunction(const SymbolTable::Function& function) {
    const std::string& name = function.name;
    LOG("Adding function for name %s", name.c_str());

    Function *f = new Function(function);
    if (functions_.count(name) == 0) {
        names_.push_back(name);
    }
    std::vector<SymbolTable::Function*>& v = functions_[name];
    v.push_back(f);

    return f;
}
