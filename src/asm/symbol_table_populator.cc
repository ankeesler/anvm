#include "symbol_table_populator.h"

#include <assert.h>
#include <string>
#include <vector>

#include "symbol_table.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("symbol_table_populator", __FILE__, __LINE__, __VA_ARGS__)

static bool ConvertStringToWord(const std::string& name, Word *word);
static bool IsValidRegister(const std::string& name);

static std::map<std::string, std::pair<Word, int>> instruction_map = {
    { "LOAD", std::pair<Word, int>(ILOAD, 2) },
    { "STORE", std::pair<Word, int>(ISTORE, 2) },
    { "BRANCH", std::pair<Word, int>(IBRANCH, 1) },
    { "ADD", std::pair<Word, int>(IADD, 0) },
    { "SUBTRACT", std::pair<Word, int>(ISUBTRACT, 0) },
    { "MULTIPLY", std::pair<Word, int>(IMULTIPLY, 0) },
    { "DIVIDE", std::pair<Word, int>(IDIVIDE, 0) },
    { "IEXIT", std::pair<Word, int>(IEXIT, 0) },
};

void SymbolTablePopulator::OnStart() {
    ResetState();
}

void SymbolTablePopulator::OnError(const std::string& s, int line_num) {
    LOG("ERROR: line %d: %s", line_num, s.c_str());
}

void SymbolTablePopulator::OnFunction(const std::string& name, int line_num) {
    LOG("Function %s found on line %d", name.c_str(), line_num);

    Word address = (state_.function != nullptr
            ? state_.function->words.size()
            : 0);
    Symbol s(name, true, address, Symbol::FUNCTION);
    s.name = name;
    state_.function = st_->AddSymbol(s);
}

void SymbolTablePopulator::OnInstruction(const std::string& name, int line_num) {
    LOG("Instruction %s found on line %d", name.c_str(), line_num);

    if (state_.function == nullptr) {
        Error e;
        e << "Received instruction '" << name << "' before function on line " << line_num;
        state_.errors.push_back(e);
        return;
    }

    if (state_.instruction != IEXIT && state_.actual_args != state_.expected_args) {
        Error e;
        e << "Partial statement on line " << state_.line_num;
        e << ": expected " << state_.expected_args << " args but got " << state_.actual_args;
        state_.errors.push_back(e);
        // keep going
    }

    if (instruction_map.count(name) == 0) {
        Error e;
        e << "Invalid instruction '" << name << "' found on line " << line_num;
        state_.errors.push_back(e);
        return;
    }

    std::pair<Word, int>& instruction_info = instruction_map[name];
    state_.line_num = line_num;
    state_.instruction = instruction_info.first;
    state_.expected_args = instruction_info.second;
    state_.actual_args = 0;
    state_.function->words.push_back(state_.instruction);
}

void SymbolTablePopulator::OnArg(enum ArgType type, const std::string& name, int line_num) {
    LOG("Arg %d/%s found on line %d", type, name.c_str(), line_num);

    if (state_.function == nullptr) {
        Error e;
        e << "Received arg '" << name << "' before function on line " << line_num;
        state_.errors.push_back(e);
        return;
    }

    if (state_.function->words.size() == 0) {
        Error e;
        e << "Received arg '" << name << "' before instruction on line " << line_num;
        state_.errors.push_back(e);
        return;
    }

    if (state_.expected_args == state_.actual_args) {
        Error e;
        e << "Too many args in statement on line " << state_.line_num;
        e << ": expected " << state_.expected_args << " args but got " << state_.actual_args+1;
        state_.errors.push_back(e);
        return;
    }

    if (type == Parser::Handler::REGISTER && !IsValidRegister(name)) {
        Error e;
        e << "Invalid register '" << name << "' found on line " << line_num;
        state_.errors.push_back(e);
        state_.actual_args++;
        return;
    }

    std::vector<Word>* words = &state_.function->words;
    int lastWordIndex = words->size() - 1;
    Word w;
    if (state_.actual_args == 0) {
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
                    state_.errors.push_back(e);
                    return;
                }
                words->at(lastWordIndex) = w;
                break;

            case IBRANCH:
                if (!GetBranchInstruction(type, line_num, &w)) {
                    Error e;
                    e << "Could not match BRANCH instruction with type " << type;
                    state_.errors.push_back(e);
                    return;
                }
                words->at(lastWordIndex) = w;
                break;
        }
    }

    if (type == Parser::Handler::SYMBOL) {
        w = 0; // linker fills in later
        st_->AddSymbol(Symbol(name, false, words->size(), Symbol::FUNCTION));
    } else {
        ConvertStringToWord(name, &w);
    }
    words->push_back(w);
    if (++state_.actual_args == state_.expected_args) {
        state_.instruction = IEXIT;
    }
}

void SymbolTablePopulator::OnEnd() {
    if (state_.expected_args != state_.actual_args) {
        Error e;
        e << "Partial statement on last line: expected " << state_.expected_args << " args but got " << state_.actual_args;
        state_.errors.push_back(e);
        return;
    }
}

void SymbolTablePopulator::ResetState() {
    state_.errors.clear();
    state_.line_num = 0;
    state_.function = nullptr;
    state_.instruction = IEXIT;
    state_.expected_args = 0;
    state_.actual_args = 0;
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
        case Parser::Handler::SYMBOL:
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
