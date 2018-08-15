#ifndef ANVM_ASSEMBLER_PARSER_H_
#define ANVM_ASSEMBLER_PARSER_H_

#include <istream>
#include <vector>
#include <map>

#include "src/util/log.h"

class Parser {
    public:
        class Handler {
            public:
                enum ArgType {
                    LITERAL,
                    REFERENCE,
                    REGISTER,
                    REGISTER_REFERENCE,
                    SYMBOL,
                };

                virtual void OnStart() = 0;
                virtual void OnError(const std::string& s, int line_num) = 0;
                virtual void OnFunction(const std::string& name, int line_num) = 0;
                virtual void OnInstruction(const std::string& name, int line_num) = 0;
                virtual void OnArg(enum ArgType type, const std::string& name, int line_num) = 0;
                virtual void OnEnd() = 0;
        };

        Parser(Log *log) : log_(log) { }

        void Parse(std::istream& is, Handler *handler);

    private:
        bool ParseFunction(const std::string& line, int line_num, Handler *handler);
        void ParseStatement(const std::string& line, int line_num, Handler *handler);
        bool ParseLiteralArg(const std::string& arg, int line_num, Handler *handler);
        bool ParseReferenceArg(const std::string& arg, int line_num, Handler *handler);
        bool ParseRegisterArg(const std::string& arg, int line_num, bool is_reference, Handler *handler);
        bool ParseSymbolArg(const std::string& arg, int line_num, Handler *handler);

        Log *log_;
        bool error_;
};

#endif // ANVM_ASSEMBLER_PARSER_H_
