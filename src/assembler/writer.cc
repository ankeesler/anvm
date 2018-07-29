#include "writer.h"

#include <assert.h>
#include <ostream>
#include <vector>

#include "parser.h"
#include "log.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("writer", __FILE__, __LINE__, __VA_ARGS__)

static bool ParseToken(const std::string& token, Word *literal); 

const char *Writer::Write(const Parser::Result& result, std::vector<Word>* output) {
    LOG("Starting writer");

    for (const Parser::Function& function : result.Functions()) {
        const char *error = WriteFunction(function, output);
        if (error != nullptr) {
            return error;
        }
    }

    return nullptr;
}

const char *Writer::WriteFunction(const Parser::Function& function, std::vector<Word>* output) {
    LOG("Writing function: %s", function.Name().c_str());

    for (const Parser::Statement& statement : function.Statements()) {
        const char *error = WriteStatement(statement, output);
        if (error != nullptr) {
            return error;
        }
    }

    return nullptr;
} 

const char *Writer::WriteStatement(const Parser::Statement& statement, std::vector<Word>* output) {
    LOG("Writing statement: %s", statement.Instruction().c_str());

    const std::string& instruction = statement.Instruction(); 
    if (strcmp(instruction.c_str(), "LOAD") == 0) {
        return WriteLoadStatement(statement, output);
    }

    return nullptr;
} 

const char *Writer::WriteLoadStatement(const Parser::Statement& statement, std::vector<Word>* output) {
    const std::vector<Parser::Arg>& args = statement.Args();
    if (args.size() != 2) {
        LOG("ERROR: LOAD expects 2 args, got %d", args.size());
        assert(0);
    }

    Word instructionWord;
    Word argWords[2];
    switch (args[0].Type()) {
        case Parser::Arg::Type::LITERAL:
            instructionWord = ILOAD;
            break;

        case Parser::Arg::Type::REGISTER:
            instructionWord = ILOADR;
            break;

        case Parser::Arg::Type::REFERENCE:
            instructionWord = ILOADM;
            break;

        default:
            LOG("ERROR: unexpected type of arg: %d", args[0].Type());
            assert(0);
    }

    if (!ParseToken(args[0].Token(), &argWords[0])) {
        LOG("ERROR: cannot parse register from token: %s", args[0].Token().c_str());
        assert(0);
    }

    if (!ParseToken(args[1].Token(), &argWords[1])) {
        LOG("ERROR: cannot parse register from token: %s", args[1].Token().c_str());
        assert(0);
    }

    output->push_back(instructionWord);
    output->push_back(argWords[0]);
    output->push_back(argWords[1]);

    return nullptr;
}

static bool ParseToken(const std::string& token, Word *literal) {
    try {
        long long value = std::stoll(token);
        if (value > static_cast<long long>(INT32_MAX)) {
            return false;
        }
        *literal = static_cast<Word>(value);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
} 
