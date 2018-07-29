#include "writer.h"

#include <assert.h>
#include <ostream>
#include <vector>
#include <map>

#include "parser.h"
#include "log.h"
#include "src/cpu.h"

#define LOG(...) log_->Printf("writer", __FILE__, __LINE__, __VA_ARGS__)

static std::map<std::string, Word> instruction_map = {
    { "ADD", IADD, },
    { "MULTIPLY", IMULTIPLY, },
};

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
    if (instruction == "LOAD") {
        return WriteLoadStatement(statement, output);
    } else if (instruction == "STORE") {
        return WriteStoreStatement(statement, output);
    } else {
        if (instruction_map.count(instruction) == 0) {
            assert(0);
        }
        Word instructionWord = instruction_map[instruction];
        output->push_back(instructionWord);
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

    output->push_back(instructionWord);
    for (const Parser::Arg& arg : args) {
        output->push_back(arg.Value());
    }

    return nullptr;
}

const char *Writer::WriteStoreStatement(const Parser::Statement& statement, std::vector<Word>* output) {
    const std::vector<Parser::Arg>& args = statement.Args();
    if (args.size() != 2) {
        LOG("ERROR: STORE expects 2 args, got %d", args.size());
        assert(0);
    }

    Word instructionWord;
    switch (args[0].Type()) {
        case Parser::Arg::Type::LITERAL:
            instructionWord = ISTORE;
            break;

        case Parser::Arg::Type::REGISTER:
            instructionWord = ISTORER;
            break;

        default:
            LOG("ERROR: unexpected type of arg: %d", args[0].Type());
            assert(0);
    }

    output->push_back(instructionWord);
    for (const Parser::Arg& arg : args) {
        output->push_back(arg.Value());
    }

    return nullptr;
}
