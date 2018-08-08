#include <vector>
#include <sstream>
#include <map>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/assembler/parser.h"
#include "src/util/log.h"

using testing::Sequence;
using testing::StrictMock;

class TestHandler : public Parser::Handler {
    public:
        MOCK_METHOD0(OnStart, void());
        MOCK_METHOD2(OnError, void(const std::string& name, int line_num));
        MOCK_METHOD2(OnFunction, void(const std::string& name, int line_num));
        MOCK_METHOD2(OnInstruction, void(const std::string& name, int line_num));
        MOCK_METHOD3(OnArg, void(enum Parser::Handler::ArgType type, const std::string& name, int line_num));
        MOCK_METHOD0(OnEnd, void());
};

class BasicParserTest : public testing::Test {
    public:
        BasicParserTest() {
            log_ = new StdoutLog();
            parser_ = new Parser(log_);
            const std::string text =
                "tuna:\n"
                "  LOAD #1 %r2\n"
                "\n"
                ";; here is a comment\n"
                "fish: ;; here is another comment\n"
                "  STORE #1 @2\n"
                "  ADD      ;; here is yet another comment\n"
                "  BRANCH $tuna\n"
                "  LOAD @4 %rpc\n"
                "\n"
                "marlin:\n"
                "  ;; here is srsly another comment\n"
                "  LOAD #1 %r0\n"
                "  LOAD @%r2 %r1\n"
                "  MULTIPLY\n"
                "  LOAD #0 %rsp\n";
            is_ = new std::istringstream(text, std::ios_base::in);
        }

        ~BasicParserTest() {
            delete log_;
            delete parser_;
            delete is_;
        }

    protected:
        StdoutLog *log_;
        Parser *parser_;
        std::istringstream *is_;
        StrictMock<TestHandler> handler_;
};

TEST_F(BasicParserTest, KitchenSink) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "2", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("fish", 5)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("STORE", 6)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 6)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REFERENCE, "2", 6)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("ADD", 7)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("BRANCH", 8)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::SYMBOL, "tuna", 8)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 9)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REFERENCE, "4", 9)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "pc", 9)).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("marlin", 11)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 13)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 13)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "0", 13)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 14)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER_REFERENCE, "2", 14)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "1", 14)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("MULTIPLY", 15)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 16)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "0", 16)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "sp", 16)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    parser_->Parse(*is_, &handler_);
}

// Section: general negative tests

TEST_F(BasicParserTest, SpacesInLine) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "2", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("fish", 5)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("STORE", 7)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 7)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REFERENCE, "2", 7)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD #1 %r2\n"
        "\n"
        "  \n"
        "fish:\n"
        "  \n"
        "  STORE #1 @2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, NoColon) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::LITERAL, "1", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REGISTER, "2", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Expected colon at end of declaration of function fish", 4)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD #1 %r2\n"
        "\n"
        "fish\n"
        "  STORE #1 @2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

// Section: bad literal args

TEST_F(BasicParserTest, BadLiteral) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Invalid literal #a", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD #a %r2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, EmptyLiteral) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Empty literal #", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD # %r2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, TooBigLiteral) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Invalid literal #12147483647", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "LOAD #12147483647 %r0\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

// Section: bad reference args

TEST_F(BasicParserTest, BadReference) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Invalid reference @tuna", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD @tuna %r2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, EmptyReference) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Empty reference @", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD @ %r2\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, TooBigReference) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Invalid reference @12147483647", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "LOAD @12147483647 %r0\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

// Section: bad register args

TEST_F(BasicParserTest, EmptyRegister) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REFERENCE, "5", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Empty register %r", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD @5 %r\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

TEST_F(BasicParserTest, PartialRegister) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("LOAD", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnArg(Parser::Handler::REFERENCE, "5", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Invalid syntax % (did you mean '%r')", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  LOAD @5 %\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}

// Section: bad symbol args

TEST_F(BasicParserTest, EmptySymbol) {
    Sequence s;
    EXPECT_CALL(handler_, OnStart()).InSequence(s);
    EXPECT_CALL(handler_, OnFunction("tuna", 1)).InSequence(s);
    EXPECT_CALL(handler_, OnInstruction("BRANCH", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnError("Empty symbol $", 2)).InSequence(s);
    EXPECT_CALL(handler_, OnEnd()).InSequence(s);

    const std::string text =
        "tuna:\n"
        "  BRANCH $\n";
    std::istringstream iss(text);
    parser_->Parse(iss, &handler_);
}
