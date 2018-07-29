#ifndef ANVM_ASSEMBLER_PARSER_H_
#define ANVM_ASSEMBLER_PARSER_H_

#include <istream>
#include <vector>
#include <map>

#include "log.h"

class Parser {
    public:
        class Arg {
            public:
                enum Type {
                    LITERAL,
                    REGISTER,
                    REFERENCE,
                    REGISTER_REFERENCE,
                };
                const std::string& Token() const { return token_; }
                void SetToken(const std::string& token) { token_ = token; }

                enum Type Type() const { return type_; }
                void SetType(enum Type type) { type_ = type; }

            private:
                std::string token_;
                enum Type type_;
        };

        class Statement {
            public:
                const std::string& Instruction() const { return instruction_; }
                void SetInstruction(const std::string& instruction) { instruction_ = instruction; }

                const std::vector<Arg> Args() const { return args_; }
                void AddArg(const Arg& arg) { args_.push_back(arg); }

            private:
                std::string instruction_;
                std::vector<Arg> args_;
        };

        class Function {
            public:
                const std::string& Name() const { return name_; }
                void SetName(const std::string& name) { name_ = name; }

                const std::vector<Statement>& Statements() const { return statements_; }
                void AddStatement(const Statement& statement) { statements_.push_back(statement); }

            private:
                std::string name_;
                std::vector<Statement> statements_;
        };

        class Result {
            public:
                Result() : error_(false) { }

                void Clear() { functions_.clear(); }

                const std::vector<Function>& Functions() const { return functions_; }
                void AddFunction(const Function& function) { functions_.push_back(function); }

                bool Error(std::string *error) const {
                    if (error_ && error != nullptr) {
                        *error = errorString_;
                    }
                    return error_;
                }
                void SetError(const std::string& error) {
                    error_ = true;
                    errorString_ = error;
                }

            private:
                bool error_;
                std::string errorString_;
                std::vector<Function> functions_;
        };

        Parser(Log *log) : log_(log) { }

        const Result& Parse(std::istream& is);

    private:
        void ParseFunction(const std::string& line);
        void AddFunction();
        void ParseAndAddStatement(const std::string& line);

        Log *log_;
        Result result_;
        Function *currentFunction_;
};

#endif // ANVM_ASSEMBLER_PARSER_H_
