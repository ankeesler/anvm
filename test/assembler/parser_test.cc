#include <vector>
#include <sstream>
#include <map>

#include "gtest/gtest.h"

#include "src/assembler/parser.h"
#include "src/util/log.h"
#include "src/util/error.h"

class BasicParserTest : public testing::Test {
    public:
        BasicParserTest() {
            StdoutLog log;
            Parser p(&log);
            std::string text =
                "tuna:\n"
                "  LOAD 1 2\n"
                "\n"
                ";; here is a comment\n"
                "fish: ;; here is another comment\n"
                "  STORE 1 @2\n"
                "  ADD      ;; here is yet another comment\n"
                "\n"
                "marlin:\n"
                "  ;; here is srsly another comment\n"
                "  LOAD 1 %r0\n"
                "  LOAD @%r2 1\n"
                "  MULTIPLY\n"
                "  LOAD 0 %rsp\n";
            std::stringstream is(text, std::ios_base::in);
            p.Parse(is, &result_);
        }

    protected:
        Parser::Result result_;
};

std::map<std::string, const Parser::Function*> *CollectFunctionMap(const std::vector<Parser::Function>& functions) {
    std::map<std::string, const Parser::Function*> *m = new std::map<std::string, const Parser::Function*>();
    for (unsigned int i = 0; i < functions.size(); i++) {
        (*m)[functions[i].Name()] = &functions[i];
    }
    return m;
}

std::map<std::string, const Parser::Statement*> *CollectStatementMap(const Parser::Function& function) {
    std::map<std::string, const Parser::Statement*> *m = new std::map<std::string, const Parser::Statement*>();
    const std::vector<Parser::Statement> statements = function.Statements();
    for (unsigned int i = 0; i < statements.size(); i++) {
        (*m)[statements[i].Instruction()] = &statements[i];
    }
    return m;
}

TEST_F(BasicParserTest, FunctionNames) {
    EXPECT_EQ(result_.Errors().size(), 0);

    auto functions = result_.Functions();
    EXPECT_EQ(functions.size(), 3);

    auto functionMap = CollectFunctionMap(functions);
    EXPECT_TRUE(functionMap->count("tuna") > 0);
    EXPECT_TRUE(functionMap->count("fish") > 0);
    EXPECT_TRUE(functionMap->count("marlin") > 0);
}

TEST_F(BasicParserTest, StatementInstructions) {
    EXPECT_EQ(result_.Errors().size(), 0);

    auto functionMap = CollectFunctionMap(result_.Functions());

    const Parser::Function *function = functionMap->at("tuna");
    auto statements = function->Statements();
    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(statements[0].Instruction(), "LOAD");

    function = functionMap->at("fish");
    statements = function->Statements();
    EXPECT_EQ(statements.size(), 2);
    EXPECT_EQ(statements[0].Instruction(), "STORE");
    EXPECT_EQ(statements[1].Instruction(), "ADD");

    function = functionMap->at("marlin");
    statements = function->Statements();
    EXPECT_EQ(statements.size(), 4);
    EXPECT_EQ(statements[0].Instruction(), "LOAD");
    EXPECT_EQ(statements[1].Instruction(), "LOAD");
    EXPECT_EQ(statements[2].Instruction(), "MULTIPLY");
    EXPECT_EQ(statements[3].Instruction(), "LOAD");
}

TEST_F(BasicParserTest, StatementArgs) {
    EXPECT_EQ(result_.Errors().size(), 0);

    auto functionMap = CollectFunctionMap(result_.Functions());

    const Parser::Function *function = functionMap->at("tuna");
    auto statements = function->Statements();
    auto args = statements[0].Args();
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].Value(), 1);
    EXPECT_EQ(args[1].Value(), 2);

    function = functionMap->at("fish");
    statements = function->Statements();
    args = statements[0].Args();
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].Value(), 1);
    EXPECT_EQ(args[0].Type(), Parser::Arg::LITERAL);
    EXPECT_EQ(args[1].Value(), 2);
    EXPECT_EQ(args[1].Type(), Parser::Arg::REFERENCE);
    args = statements[1].Args();
    EXPECT_EQ(args.size(), 0);

    function = functionMap->at("marlin");
    statements = function->Statements();
    args = statements[0].Args();
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].Value(), 1);
    EXPECT_EQ(args[0].Type(), Parser::Arg::LITERAL);
    EXPECT_EQ(args[1].Value(), 0);
    EXPECT_EQ(args[1].Type(), Parser::Arg::REGISTER);
    args = statements[1].Args();
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].Value(), 2);
    EXPECT_EQ(args[0].Type(), Parser::Arg::REGISTER_REFERENCE);
    EXPECT_EQ(args[1].Value(), 1);
    EXPECT_EQ(args[1].Type(), Parser::Arg::LITERAL);
    args = statements[2].Args();
    EXPECT_EQ(args.size(), 0);
    args = statements[3].Args();
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].Value(), 0);
    EXPECT_EQ(args[0].Type(), Parser::Arg::LITERAL);
    EXPECT_EQ(args[1].Value(), -2);
    EXPECT_EQ(args[1].Type(), Parser::Arg::REGISTER);
}

TEST(ParserTest, BadStatement) {
    StdoutLog log;
    Parser p(&log);
    std::string text =
        "tuna:\n"
        "  LOAD 1 2\n"
        "\n"
        "fish:\n"
        "  \n"
        "\n"
        "marlin:\n"
        "  LOAD 1 %r0\n"
        "  LOAD @%r2 1\n"
        "  MULTIPLY\n"
        "  LOADR 0 %rsp\n";
    std::stringstream is(text, std::ios_base::in);
    Parser::Result result;
    p.Parse(is, &result);
    std::string errorString;
    EXPECT_EQ(result.Errors().size(), 1);
    EXPECT_EQ(result.Errors()[0].S(), "ERROR: line 5: no instruction in statement:   ");;
}

TEST(ParserTest, PartialRegisterSyntax) {
    StdoutLog log;
    Parser p(&log);
    std::string text =
        "tuna:\n"
        "  LOAD 1 2\n"
        "\n"
        "fish:\n"
        "  LOAD 1 2\n"
        "\n"
        "marlin:\n"
        "  LOAD 1 %r0\n"
        "  LOAD @%2 1\n"
        "  MULTIPLY\n"
        "  LOADR 0 %rsp\n";
    std::stringstream is(text, std::ios_base::in);
    Parser::Result result;
    p.Parse(is, &result);
    std::string errorString;
    EXPECT_EQ(result.Errors().size(), 1);
    EXPECT_EQ(result.Errors()[0].S(), "ERROR: line 9: expected register identifier at character 2 of token @%2");
}

TEST(ParserTest, NoColonFunction) {
    StdoutLog log;
    Parser p(&log);
    std::string text =
        "tuna:\n"
        "  LOAD 1 2\n"
        "\n"
        "fish\n"
        "  LOAD 1 2\n"
        "\n"
        "marlin:\n"
        "  LOAD 1 %r0\n"
        "  LOAD @%2 1\n"
        "  MULTIPLY\n"
        "  LOADR 0 %rsp\n";
    std::stringstream is(text, std::ios_base::in);
    Parser::Result result;
    p.Parse(is, &result);
    EXPECT_EQ(result.Errors().size(), 1);
    EXPECT_EQ(result.Errors()[0].S(), "ERROR: line 4: expected colon at end of declaration of function fish");
}
