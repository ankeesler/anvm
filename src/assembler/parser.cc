#include "parser.h"

#include <assert.h>
#include <istream>
#include <sstream>
#include <string>

#include "src/cpu.h"
#include "src/util/log.h"

#define LOG(...) log_->Printf("parser", __FILE__, __LINE__, __VA_ARGS__)

#define LogComment(line, line_num) \
    LOG("Encountered comment as first entry of line %d (%s)", (line_num), (line).c_str());

static bool ParseValue(const std::string& token, Word *value);
static bool IsComment(const std::string& s);

void Parser::Parse(std::istream& is, Parser::Result *result) {
    LOG("Starting to parse input stream");
    currentFunction_ = nullptr;

    std::string line;
    int line_num = 1;
    while (std::getline(is, line)) {
        LOG("Read line: %s", line.c_str());
        if (line.size() == 0) {
            AddFunction(result);
        } else if (currentFunction_ == nullptr) {
            ParseFunction(line, line_num, result);
        } else {
            ParseAndAddStatement(line, line_num, result);
        }

        if (result->Errors().size() > 0) {
            return;
        }

        line_num++;
    }

    if (currentFunction_ != nullptr) {
        AddFunction(result);
    }
}

void Parser::ParseFunction(const std::string& line, int line_num, Parser::Result *result) {
    LOG("Parsing function");

    std::istringstream iss(line);
    std::string first_symbol;

    if (!(iss >> first_symbol)) {
        assert(0);
    }

    if (IsComment(first_symbol)) {
        LogComment(line, line_num);
        return;
    }

    if (first_symbol.back() == ':') {
        const std::string&& name = first_symbol.substr(0, first_symbol.size()-1);
        currentFunction_ = new Function();
        currentFunction_->SetName(name);
    } else {
        Error e;
        e << "ERROR: line " << line_num << ": ";
        e << "expected colon at end of declaration of function " << line;
        LOG(e.S().c_str());
        result->AddError(e);
    }
}

void Parser::AddFunction(Parser::Result *result) {
    result->AddFunction(*currentFunction_);
    LOG("Added function with name %s", currentFunction_->Name().c_str());

    delete currentFunction_;
    currentFunction_ = nullptr;
}

void Parser::ParseAndAddStatement(const std::string& line, int line_num, Parser::Result *result) {
    std::stringstream ss(line);
    std::string first_symbol;
    if (!(ss >> first_symbol)) {
        Error e;
        e << "ERROR: line " << line_num << ": ";
        e << "no instruction in statement: " << line;
        LOG(e.S().c_str());
        result->AddError(e);
        return;
    }

    if (IsComment(first_symbol)) {
        LogComment(line, line_num);
        return;
    }

    const std::string& instruction = first_symbol;
    Statement statement;
    statement.SetInstruction(instruction);
    LOG("Setting statement instruction: %s", instruction.c_str());

    std::string token;
    while (ss >> token) {
        if (IsComment(token)) {
            LogComment(line, line_num);
            break;
        }

        Arg a;
        bool isReference = false;
        bool isRegister = false;
        int tokenStart = 0;
        if (token.at(tokenStart) == '@') {
            isReference = true;
            tokenStart++;
        }
        if (token.at(tokenStart) == '%') {
            if (token.at(tokenStart+1) != 'r') {
                Error e;
                e << "ERROR: line " << line_num << ": ";
                e << "expected register identifier at character " << tokenStart+1 << " of token " << token;
                LOG(e.S().c_str());
                result->AddError(e);
                return;
            }
            isRegister = true;
            tokenStart += 2;
        }

        const std::string&& realToken = token.substr(tokenStart);
        Word value;
        if (!ParseValue(realToken, &value)) {
            Error e;
            e << "ERROR: line " << line_num << ": ";
            e << "failed to parse value from token " << realToken.c_str();
            LOG(e.S().c_str());
            assert(0);
        }

        enum Parser::Arg::Type type;
        if (isReference && isRegister) {
            type = Parser::Arg::REGISTER_REFERENCE;
        } else if (isReference) {
            type = Parser::Arg::REFERENCE;
        } else if (isRegister) {
            type = Parser::Arg::REGISTER;
        } else {
            type = Parser::Arg::LITERAL;
        }

        a.SetValue(value);
        a.SetType(type);
        statement.AddArg(a);
        LOG("Added instruction arg token/type: %d/%d", a.Value(), a.Type());
    }

    currentFunction_->AddStatement(statement);
}

static bool ParseValue(const std::string& token, Word *value) {
    if (token == "sp") {
        *value = SP;
        return true;
    }

    try {
        long long bigValue = std::stoll(token);
        if (bigValue > static_cast<long long>(INT32_MAX)) {
            return false;
        }
        *value = static_cast<Word>(bigValue);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

static bool IsComment(const std::string& s) {
    return s.size() >= 2 && s[0] == ';' && s[1] == ';';
}
