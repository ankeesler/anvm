#include "linker.h"

#include <assert.h>
#include <ostream>
#include <vector>
#include <map>

#include "parser.h"
#include "log.h"
#include "src/cpu.h"
#include "src/program.h"

#define LOG(...) log_->Printf("linker", __FILE__, __LINE__, __VA_ARGS__)

static std::map<std::string, Word> instruction_map = {
    { "ADD", IADD, },
    { "MULTIPLY", IMULTIPLY, },
};

const char *Linker::Link(const Parser::Result& result, Program *program) {
    LOG("Starting linker");

    for (const Parser::Function& function : result.Functions()) {
        const char *error = WriteFunction(function, program);
        if (error != nullptr) {
            return error;
        }
    }

    program->AddWord(IEXIT);

    LOG("Returning from linker");
    return nullptr;
}

const char *Linker::WriteFunction(const Parser::Function& function, Program *program) {
    LOG("Writing function: %s", function.Name().c_str());

    for (const Parser::Statement& statement : function.Statements()) {
        const char *error = WriteStatement(statement, program);
        if (error != nullptr) {
            return error;
        }
    }

    return nullptr;
}

const char *Linker::WriteStatement(const Parser::Statement& statement, Program *program) {
    LOG("Writing statement: %s", statement.Instruction().c_str());

    const std::string& instruction = statement.Instruction();
    if (instruction == "LOAD") {
        return WriteLoadStatement(statement, program);
    } else if (instruction == "STORE") {
        return WriteStoreStatement(statement, program);
    } else {
        if (instruction_map.count(instruction) == 0) {
            assert(0);
        }
        Word instructionWord = instruction_map[instruction];
        program->AddWord(instructionWord);
    }

    return nullptr;
}

const char *Linker::WriteLoadStatement(const Parser::Statement& statement, Program *program) {
    const std::vector<Parser::Arg>& args = statement.Args();
    if (args.size() != 2) {
        LOG("ERROR: LOAD expects 2 args, got %d", args.size());
        assert(0);
    }

    Word instructionWord;
    switch (args[0].Type()) {
        case Parser::Arg::LITERAL:
            instructionWord = ILOAD;
            break;

        case Parser::Arg::REGISTER:
            instructionWord = ILOADR;
            break;

        case Parser::Arg::REFERENCE:
            instructionWord = ILOADM;
            break;

        default:
            LOG("ERROR: unexpected type of arg: %d", args[0].Type());
            assert(0);
    }

    program->AddWord(instructionWord);
    for (const Parser::Arg& arg : args) {
        program->AddWord(arg.Value());
    }

    return nullptr;
}

const char *Linker::WriteStoreStatement(const Parser::Statement& statement, Program *program) {
    const std::vector<Parser::Arg>& args = statement.Args();
    if (args.size() != 2) {
        LOG("ERROR: STORE expects 2 args, got %d", args.size());
        assert(0);
    }

    Word instructionWord;
    switch (args[0].Type()) {
        case Parser::Arg::LITERAL:
            instructionWord = ISTORE;
            break;

        case Parser::Arg::REGISTER:
            instructionWord = ISTORER;
            break;

        default:
            LOG("ERROR: unexpected type of arg: %d", args[0].Type());
            assert(0);
    }

    program->AddWord(instructionWord);
    for (const Parser::Arg& arg : args) {
        program->AddWord(arg.Value());
    }

    return nullptr;
}
