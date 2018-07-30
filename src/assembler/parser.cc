#include "parser.h"

#include <assert.h>
#include <istream>
#include <sstream>
#include <string>

#include "log.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("parser", __FILE__, __LINE__, __VA_ARGS__)

static bool ParseValue(const std::string& token, Word *value);

const Parser::Result& Parser::Parse(std::istream& is) {
    LOG("Starting to parse input stream");
    result_.Clear();
    currentFunction_ = nullptr;

    std::string line;
    while (std::getline(is, line)) {
        LOG("Read line: %s", line.c_str());
        if (line.size() == 0) {
            AddFunction();
        } else if (currentFunction_ == nullptr) {
            ParseFunction(line);
        } else {
            ParseAndAddStatement(line);
        }

        if (result_.Error(nullptr)) {
            break;
        }
    }

    if (currentFunction_ != nullptr) {
        AddFunction();
    }

    return result_;
}

void Parser::ParseFunction(const std::string& line) {
    LOG("Parsing function");

    if (line.back() == ':') {
        const std::string&& name = line.substr(0, line.size()-1);
        currentFunction_ = new Function();
        currentFunction_->SetName(name);
    } else {
        std::stringstream errorSs;
        errorSs << "ERROR: expected colon at end of declaration of function " << line;
        result_.SetError(errorSs.str());
        LOG(errorSs.str().c_str());
        return;
    }
}

void Parser::AddFunction() {
    result_.AddFunction(*currentFunction_);
    LOG("Added function with name %s", currentFunction_->Name().c_str());

    delete currentFunction_;
    currentFunction_ = nullptr;
}

void Parser::ParseAndAddStatement(const std::string& line) {
    std::stringstream ss(line);
    std::string instruction;
    if (!(ss >> instruction)) {
        std::stringstream errorSs;
        errorSs << "ERROR: No instruction in statement: " << line;
        result_.SetError(errorSs.str());
        LOG(errorSs.str().c_str());
        return;
    }

    Statement statement;
    statement.SetInstruction(instruction);
    LOG("Setting statement instruction: %s", instruction.c_str());

    std::string token;
    while (ss >> token) {
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
                std::stringstream errorSs;
                errorSs << "ERROR: expected register identifier at character " << tokenStart+1 << " of token " << token;
                result_.SetError(errorSs.str());
                LOG(errorSs.str().c_str());
                return;
            }
            isRegister = true;
            tokenStart += 2;
        }

        const std::string&& realToken = token.substr(tokenStart);
        Word value;
        if (!ParseValue(realToken, &value)) {
            LOG("ERROR: failed to parse value from token %s", realToken.c_str());
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
