#include "parser.h"

#include <assert.h>
#include <istream>
#include <sstream>
#include <string>

#include "src/util/log.h"

#define LOG(...) log_->Printf("parser", __FILE__, __LINE__, __VA_ARGS__)

#define LogComment(line, line_num) \
    LOG("Encountered comment as first entry of line %d (%s)", (line_num), (line).c_str());

static bool IsValidValue(const std::string& token);
static bool IsComment(const std::string& s);

void Parser::Parse(std::istream& is, Parser::Handler *handler) {
    LOG("Starting to parse input stream");
    handler->OnStart();

    std::string line;
    int line_num = 1;
    bool in_function = false;
    error_ = false;
    while (std::getline(is, line)) {
        LOG("Read line: %s", line.c_str());

        if (line.size() == 0) {
            in_function = false;
        } else if (!in_function) {
            in_function = ParseFunction(line, line_num, handler);
        } else {
            ParseStatement(line, line_num, handler);
        }

        if (error_) {
            break;
        }

        line_num++;
    }

    handler->OnEnd();
    LOG("Done parsing input stream");
}

bool Parser::ParseFunction(const std::string& line, int line_num, Parser::Handler *handler) {
    LOG("Parsing function");

    std::istringstream iss(line);
    std::string first_symbol;

    if (!(iss >> first_symbol)) {
        return false; // spaces in line
    } else if (IsComment(first_symbol)) {
        LogComment(line, line_num);
        return false;
    } else if (first_symbol.back() == ':') {
        const std::string&& name = first_symbol.substr(0, first_symbol.size()-1);
        handler->OnFunction(name, line_num);
        return true;
    } else {
        std::ostringstream oss;
        oss << "Expected colon at end of declaration of function " << line;
        handler->OnError(oss.str(), line_num);
        error_ = true;
        return false;
    }
}

void Parser::ParseStatement(const std::string& line, int line_num, Parser::Handler *handler) {
    LOG("Parsing statement");

    std::stringstream ss(line);
    std::string first_symbol;
    if (!(ss >> first_symbol)) {
        return; // spaces in line
    }

    if (IsComment(first_symbol)) {
        LogComment(line, line_num);
        return;
    }

    const std::string& instruction = first_symbol;
    handler->OnInstruction(instruction, line_num);

    std::string token;
    while (ss >> token) {
        switch (token[0]) {
            case '#':
                if (!ParseLiteralArg(token, line_num, handler)) {
                    error_ = true;
                    return;
                }
                break;

            case '@':
                if (!ParseReferenceArg(token, line_num, handler)) {
                    error_ = true;
                    return;
                }
                break;

            case '%':
                if (!ParseRegisterArg(token, line_num, false, handler)) {
                    error_ = true;
                    return;
                }
                break;

            case '$':
                if (!ParseSymbolArg(token, line_num, handler)) {
                    error_ = true;
                    return;
                }
                break;
        }
    }
}

bool Parser::ParseLiteralArg(const std::string& arg, int line_num, Parser::Handler *handler) {
    LOG("Parsing literal arg %s", arg.c_str());

    const std::string value = arg.substr(1);
    if (value.size() == 0) {
        std::ostringstream oss;
        oss << "Empty literal " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else if (!IsValidValue(value)) {
        std::ostringstream oss;
        oss << "Invalid literal " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else {
        handler->OnArg(Parser::Handler::LITERAL, value, line_num);
    }

    return true;
}

bool Parser::ParseReferenceArg(const std::string& arg, int line_num, Parser::Handler *handler) {
    LOG("Parsing reference arg %s", arg.c_str());

    const std::string value = arg.substr(1);
    if (value.size() == 0) {
        std::ostringstream oss;
        oss << "Empty reference " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else if (value[0] == '%') {
        return ParseRegisterArg(value, line_num, true, handler);
    } else if (!IsValidValue(value)) {
        std::ostringstream oss;
        oss << "Invalid reference " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else {
        handler->OnArg(Parser::Handler::REFERENCE, value, line_num);
    }

    return true;
}

bool Parser::ParseRegisterArg(const std::string& arg, int line_num, bool is_reference, Parser::Handler *handler) {
    LOG("Parsing register arg %s%s", arg.c_str(), is_reference ? " (reference)" : "");

    if (arg.size() == 1) {
        std::ostringstream oss;
        oss << "Invalid syntax " << arg << " (did you mean '%r')";
        handler->OnError(oss.str(), line_num);
        return false;
    } else if (arg.size() == 2) {
        std::ostringstream oss;
        oss << "Empty register " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else {
        handler->OnArg((is_reference ? Parser::Handler::REGISTER_REFERENCE : Parser::Handler::REGISTER),
                arg.substr(2), line_num);
    }

    return true;
}

bool Parser::ParseSymbolArg(const std::string& arg, int line_num, Parser::Handler *handler) {
    LOG("Parsing literal arg %s", arg.c_str());

    const std::string value = arg.substr(1);
    if (value.size() == 0) {
        std::ostringstream oss;
        oss << "Empty symbol " << arg;
        handler->OnError(oss.str(), line_num);
        return false;
    } else {
        handler->OnArg(Parser::Handler::SYMBOL, value, line_num);
    }
    return true;
}

static bool IsValidValue(const std::string& token) {
    try {
        long long bigValue = std::stoll(token);
        if (bigValue > static_cast<long long>(INT32_MAX)) {
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

static bool IsComment(const std::string& s) {
    return s.size() >= 2 && s[0] == ';' && s[1] == ';';
}
