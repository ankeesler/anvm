#include "src/assembler/symbol_table_populator.h"

#include <assert.h>
#include <string>

#include "symbol_table.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("symbol_table_populator", __FILE__, __LINE__, __VA_ARGS__)

static std::map<std::string, Word> instruction_map = {
    { "ADD", IADD, },
    { "MULTIPLY", IMULTIPLY, },
    { "DIVIDE", IDIVIDE, },
    { "SUBTRACT", ISUBTRACT, },
};

void SymbolTablePopulator::OnError(const std::string& s, int line_num) {
    LOG("ERROR: line %d: %s", line_num, s.c_str());
}

void SymbolTablePopulator::OnFunction(const std::string& name, int line_num) {
    LOG("Function %s found on line %d", name.c_str(), line_num);

    SymbolTable::Function f;
    f.SetName(name);
    currentFunction_ = st_->AddFunction(f);
}

void SymbolTablePopulator::OnInstruction(const std::string& name, int line_num) {
    LOG("Instruction %s found on line %d", name.c_str(), line_num);

    Word w;
    if (name == "LOAD") {
        if (!GetLoadInstruction(name, line_num, &w)) {
            assert(0);
        }
    } else {
        w = instruction_map[name];
    }

    currentFunction_->AddWord(w);
}

void SymbolTablePopulator::OnArg(enum ArgType type, const std::string& name, int line_num) {
    LOG("Arg %d/%s found on line %d", type, name.c_str(), line_num);
}

bool SymbolTablePopulator::GetLoadInstruction(const std::string& name, int line_num, Word *w) {
    return true;
}
