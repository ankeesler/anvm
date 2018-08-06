#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/cpu.h"
#include "src/assembler/symbol_table.h"
#include "src/assembler/symbol_table_populator.h"
#include "src/util/log.h"
#include "src/util/error.h"

using testing::SizeIs;
using testing::Eq;
using testing::ElementsAreArray;
using testing::ElementsAre;
using testing::IsEmpty;

// TODO: How do we assert that the target vm has the correct registers?

TEST(SymbolTablePopulatorTest, HappyPath) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "0", 0);
    stp.OnInstruction("BRANCH", 0);
    stp.OnArg(Parser::Handler::LITERAL, "20", 0);
    stp.OnInstruction("MULTIPLY", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::REGISTER, "2", 0);
    stp.OnArg(Parser::Handler::REGISTER, "2", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 0);
    stp.OnArg(Parser::Handler::REGISTER, "3", 0);

    stp.OnFunction("fish", 0);
    stp.OnInstruction("STORE", 0);
    stp.OnArg(Parser::Handler::LITERAL, "100", 0);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 0);
    stp.OnInstruction("ADD", 0);
    stp.OnInstruction("STORE", 0);
    stp.OnArg(Parser::Handler::REGISTER, "1", 0);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 0);
    stp.OnInstruction("DIVIDE", 0);
    stp.OnInstruction("SUBTRACT", 0);

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<SymbolTable::Function*>& tunaFunctions = st.FindFunctions("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    Word tunaWords[] = { ILOAD, 1, 0, IBRANCHX, 20, IMULTIPLY, ILOADR, 2, 2, ILOADM, 64, 3, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));

    const std::vector<SymbolTable::Function*>& fishFunctions = st.FindFunctions("fish");
    EXPECT_THAT(fishFunctions, SizeIs(1));
    EXPECT_THAT(fishFunctions[0]->name, Eq("fish"));
    Word fishWords[] = { ISTORE, 100, 64, IADD, ISTORER, 1, 64, IDIVIDE, ISUBTRACT, };
    EXPECT_THAT(fishFunctions[0]->words, ElementsAreArray(fishWords, sizeof(fishWords)/sizeof(fishWords[0])));
}

TEST(SymbolTablePopulatorTest, SpecialRegisters) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "sp", 0);
    stp.OnInstruction("STORE", 0);
    stp.OnArg(Parser::Handler::REGISTER, "pc", 0);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 0);

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<SymbolTable::Function*>& tunaFunctions = st.FindFunctions("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    Word tunaWords[] = { ILOAD, 1, -2, ISTORER, -3, 64, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));
}

// Section - negative tests

TEST(SymbolTablePopulatorTest, InvalidInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "0", 0);
    stp.OnInstruction("INVALID INSTRUCTION", 5);

    EXPECT_THAT(stp.Errors(), ElementsAre(Error("Invalid instruction 'INVALID INSTRUCTION' found on line 5")));
}

TEST(SymbolTablePopulatorTest, InvalidRegister) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "0", 0);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::LITERAL, "1", 6);
    stp.OnArg(Parser::Handler::REGISTER, "invalid register", 6);
    stp.OnInstruction("LOAD", 7);
    stp.OnArg(Parser::Handler::LITERAL, "1", 7);
    stp.OnArg(Parser::Handler::REGISTER, "-1", 7);

    Error e1("Invalid register 'invalid register' found on line 6");
    Error e2("Invalid register '-1' found on line 7");
    EXPECT_THAT(stp.Errors(), ElementsAre(e1, e2));
}

TEST(SymbolTablePopulatorTest, InstructionBeforeFunction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnInstruction("LOAD", 7);

    EXPECT_THAT(stp.Errors(), ElementsAre(Error("Received instruction 'LOAD' before function started on line 7")));
}

TEST(SymbolTablePopulatorTest, ArgBeforeFunction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnArg(Parser::Handler::LITERAL, "1", 8);

    EXPECT_THAT(stp.Errors(), ElementsAre(Error("Received arg '1' before function started on line 8")));
}

TEST(SymbolTablePopulatorTest, ArgBeforeInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 9);
    stp.OnArg(Parser::Handler::LITERAL, "2", 10);

    EXPECT_THAT(stp.Errors(), ElementsAre(Error("Received arg '2' before instruction on line 10")));
}

TEST(SymbolTablePopulatorTest, AAA) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 9);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::LITERAL, "0", 6);
    stp.OnArg(Parser::Handler::REGISTER, "2", 6);

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<SymbolTable::Function*>& tunaFunctions = st.FindFunctions("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    Word tunaWords[] = { ILOAD, 0, 2, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));
}

// We test all instructions that are in cpu.h
// On arg that doesn't make sense for instruction, we error (not enough args)
