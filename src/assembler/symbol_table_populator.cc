#include "src/assembler/symbol_table_populator.h"

#include <assert.h>
#include <string>
#include <vector>

#include "symbol_table.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("symbol_table_populator", __FILE__, __LINE__, __VA_ARGS__)

static bool ConvertStringToWord(const std::string& name, Word *word);
static bool IsValidRegister(const std::string& name);

static std::map<std::string, Word> instruction_map = {
    { "LOAD", ILOAD, },
    { "STORE", ISTORE, },
    { "BRANCH", IBRANCH, },
    { "ADD", IADD, },
    { "SUBTRACT", ISUBTRACT, },
    { "MULTIPLY", IMULTIPLY, },
    { "DIVIDE", IDIVIDE, },
};

void SymbolTablePopulator::OnStart() {
}

void SymbolTablePopulator::OnError(const std::string& s, int line_num) {
    LOG("ERROR: line %d: %s", line_num, s.c_str());
}

void SymbolTablePopulator::OnFunction(const std::string& name, int line_num) {
    LOG("Function %s found on line %d", name.c_str(), line_num);

    Symbol s(name, false, 0, Symbol::FUNCTION);
    s.name = name;
    currentFunction_ = st_->AddSymbol(s);
}

void SymbolTablePopulator::OnInstruction(const std::string& name, int line_num) {
    LOG("Instruction %s found on line %d", name.c_str(), line_num);

    if (currentFunction_ == nullptr) {
        Error e;
        e << "Received instruction '" << name << "' before function on line " << line_num;
        errors_.push_back(e);
        return;
    }

    if (instruction_map.count(name) == 0) {
        Error e;
        e << "Invalid instruction '" << name << "' found on line " << line_num;
        errors_.push_back(e);
        return;
    }

    Word w = instruction_map[name];
    currentFunction_->words.push_back(w);
}

void SymbolTablePopulator::OnArg(enum ArgType type, const std::string& name, int line_num) {
    LOG("Arg %d/%s found on line %d", type, name.c_str(), line_num);

    if (currentFunction_ == nullptr) {
        Error e;
        e << "Received arg '" << name << "' before function on line " << line_num;
        errors_.push_back(e);
        return;
    }

    if (currentFunction_->words.size() == 0) {
        Error e;
        e << "Received arg '" << name << "' before instruction on line " << line_num;
        errors_.push_back(e);
        return;
    }

    if (type == Parser::Handler::REGISTER && !IsValidRegister(name)) {
        Error e;
        e << "Invalid register '" << name << "' found on line " << line_num;
        errors_.push_back(e);
        return;
    }

    std::vector<Word>* words = &currentFunction_->words;
    int lastWordIndex = words->size() - 1;
    Word w;
    switch (words->at(lastWordIndex)) {
        case ILOAD:
            if (!GetLoadInstruction(type, line_num, &w)) {
                assert(0);
            }
            words->at(lastWordIndex) = w;
            break;

        case ISTORE:
            if (!GetStoreInstruction(type, line_num, &w)) {
                Error e;
                e << "Could not match STORE instruction with type " << type;
                errors_.push_back(e);
                return;
            }
            words->at(lastWordIndex) = w;
            break;

        case IBRANCH:
            if (!GetBranchInstruction(type, line_num, &w)) {
                assert(0);
            }
            words->at(lastWordIndex) = w;
            break;
    }

    ConvertStringToWord(name, &w);
    words->push_back(w);
}

void SymbolTablePopulator::OnEnd() {
}

bool SymbolTablePopulator::GetLoadInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w) {
    switch (type) {
        case Parser::Handler::LITERAL:
            *w = ILOAD;
            break;

        case Parser::Handler::REGISTER:
            *w = ILOADR;
            break;

        case Parser::Handler::REFERENCE:
            *w = ILOADM;
            break;

        default:
            return false;
    }
    return true;
}

bool SymbolTablePopulator::GetStoreInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w) {
    switch (type) {
        case Parser::Handler::LITERAL:
            *w = ISTORE;
            break;

        case Parser::Handler::REGISTER:
            *w = ISTORER;
            break;

        default:
            return false;
    }
    return true;
}

bool SymbolTablePopulator::GetBranchInstruction(enum Parser::Handler::ArgType type, int line_num, Word *w) {
    switch (type) {
        case Parser::Handler::LITERAL:
            *w = IBRANCHX;
            break;

        default:
            return false;
    }
    return true;
}

static bool ConvertStringToWord(const std::string& name, Word *word) {
    if (name == "sp") {
        *word = SP;
    } else if (name == "pc") {
        *word = PC;
    } else {
        try {
            *word = std::stoi(name);
        } catch (const std::exception& e) {
            return false;
        }
    }
    return true;
}

static bool IsValidRegister(const std::string& name) {
    Word w;
    if (!ConvertStringToWord(name, &w)) {
        return false;
    }
    if (w < 0 && w != SP && w != PC) {
        return false;
    }
    return true;
}
