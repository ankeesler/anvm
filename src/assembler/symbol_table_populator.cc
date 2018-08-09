#include "src/assembler/symbol_table_populator.h"

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
};

void SymbolTablePopulator::OnStart() {
    errors_.clear();
    currentFunction_ = nullptr;
    current_instruction_ = IEXIT;
    current_instruction_line_ = 0;
    current_instruction_args_ = 0;
    current_args_count_ = 0;
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

    if (current_instruction_ != IEXIT
            && current_instruction_args_ != current_args_count_) {
        Error e;
        e << "Partial statement on line " << current_instruction_line_;
        e << ": expected " << current_instruction_args_ << " args but got " << current_args_count_;
        errors_.push_back(e);
        // keep going
    }

    if (instruction_map.count(name) == 0) {
        Error e;
        e << "Invalid instruction '" << name << "' found on line " << line_num;
        errors_.push_back(e);
        return;
    }

    std::pair<Word, int>& instruction_info = instruction_map[name];
    current_instruction_ = instruction_info.first;
    current_instruction_args_ = instruction_info.second;
    current_instruction_line_ = line_num;
    current_args_count_ = 0;
    currentFunction_->words.push_back(current_instruction_);
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

    if (current_args_count_ == current_instruction_args_) {
        Error e;
        e << "Too many args in statement on line " << current_instruction_line_;
        e << ": expected " << current_instruction_args_ << " args but got " << current_args_count_+1;
        errors_.push_back(e);
        return;
    }

    if (type == Parser::Handler::REGISTER && !IsValidRegister(name)) {
        Error e;
        e << "Invalid register '" << name << "' found on line " << line_num;
        errors_.push_back(e);
        current_args_count_++;
        return;
    }

    std::vector<Word>* words = &currentFunction_->words;
    int lastWordIndex = words->size() - 1;
    Word w;
    if (current_args_count_ == 0) {
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
                    Error e;
                    e << "Could not match BRANCH instruction with type " << type;
                    errors_.push_back(e);
                    return;
                }
                words->at(lastWordIndex) = w;
                break;
        }
    }

    ConvertStringToWord(name, &w);
    words->push_back(w);
    if (++current_args_count_ == current_instruction_args_) {
        current_instruction_ = IEXIT;
    }
}

void SymbolTablePopulator::OnEnd() {
    if (current_args_count_ != current_instruction_args_) {
        Error e;
        e << "Partial statement on last line: expected " << current_instruction_args_ << " args but got " << current_args_count_;
        errors_.push_back(e);
        return;
    }
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
