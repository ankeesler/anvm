#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/cpu.h"
#include "src/asm/symbol_table.h"
#include "src/asm/symbol_table_populator.h"
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

    stp.OnStart();

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
    stp.OnInstruction("IEXIT", 134);

    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<Symbol*>& tunaFunctions = st.Symbols("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    EXPECT_THAT(tunaFunctions[0]->resolved, Eq(true));
    EXPECT_THAT(tunaFunctions[0]->address, Eq(0));
    Word tunaWords[] = { ILOAD, 1, 0, IBRANCHX, 20, IMULTIPLY, ILOADR, 2, 2, ILOADM, 64, 3, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));

    const std::vector<Symbol*>& fishFunctions = st.Symbols("fish");
    EXPECT_THAT(fishFunctions, SizeIs(1));
    EXPECT_THAT(fishFunctions[0]->name, Eq("fish"));
    EXPECT_THAT(fishFunctions[0]->resolved, Eq(true));
    EXPECT_THAT(fishFunctions[0]->address, Eq(12));
    Word fishWords[] = { ISTORE, 100, 64, IADD, ISTORER, 1, 64, IDIVIDE, ISUBTRACT, IEXIT, };
    EXPECT_THAT(fishFunctions[0]->words, ElementsAreArray(fishWords, sizeof(fishWords)/sizeof(fishWords[0])));
}

TEST(SymbolTablePopulatorTest, SpecialRegisters) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "sp", 0);
    stp.OnInstruction("STORE", 0);
    stp.OnArg(Parser::Handler::REGISTER, "pc", 0);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 0);
    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<Symbol*>& tunaFunctions = st.Symbols("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    EXPECT_THAT(tunaFunctions[0]->resolved, Eq(true));
    EXPECT_THAT(tunaFunctions[0]->address, Eq(0));
    Word tunaWords[] = { ILOAD, 1, -2, ISTORER, -3, 64, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));
}

TEST(SymbolTablePopulatorTest, FakeInstructionBug) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 9);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::LITERAL, "0", 6);
    stp.OnArg(Parser::Handler::REGISTER, "2", 6);
    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<Symbol*>& tunaFunctions = st.Symbols("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    EXPECT_THAT(tunaFunctions[0]->resolved, Eq(true));
    EXPECT_THAT(tunaFunctions[0]->address, Eq(0));
    Word tunaWords[] = { ILOAD, 0, 2, };
    EXPECT_THAT(tunaFunctions[0]->words,
            ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));
}

TEST(SymbolTablePopulatorTest, BranchToSymbol) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 1);
    stp.OnInstruction("LOAD", 2);
    stp.OnArg(Parser::Handler::LITERAL, "1", 2);
    stp.OnArg(Parser::Handler::REGISTER, "sp", 2);
    stp.OnInstruction("BRANCH", 3);
    stp.OnArg(Parser::Handler::SYMBOL, "tuna", 3);
    stp.OnFunction("fish", 4);
    stp.OnInstruction("LOAD", 5);
    stp.OnArg(Parser::Handler::LITERAL, "1", 5);
    stp.OnArg(Parser::Handler::REGISTER, "sp", 5);
    stp.OnInstruction("BRANCH", 6);
    stp.OnArg(Parser::Handler::SYMBOL, "tuna", 6);
    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<Symbol*>& tunaFunctions = st.Symbols("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(3));

    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    EXPECT_TRUE(tunaFunctions[0]->resolved);
    EXPECT_THAT(tunaFunctions[0]->address, Eq(0));
    Word tunaWords[] = { ILOAD, 1, -2, IBRANCHX, 0, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));

    EXPECT_THAT(tunaFunctions[1]->name, Eq("tuna"));
    EXPECT_FALSE(tunaFunctions[1]->resolved);
    EXPECT_THAT(tunaFunctions[1]->address, Eq(4));

    EXPECT_THAT(tunaFunctions[2]->name, Eq("tuna"));
    EXPECT_FALSE(tunaFunctions[2]->resolved);
    EXPECT_THAT(tunaFunctions[2]->address, Eq(9));
}

TEST(SymbolTablePopulatorTest, BranchToRegister) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 1);
    stp.OnInstruction("LOAD", 2);
    stp.OnArg(Parser::Handler::LITERAL, "0", 2);
    stp.OnArg(Parser::Handler::REGISTER, "1", 2);
    stp.OnInstruction("BRANCH", 3);
    stp.OnArg(Parser::Handler::REGISTER, "1", 3);
    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), IsEmpty());

    const std::vector<Symbol*>& tunaFunctions = st.Symbols("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));

    EXPECT_THAT(tunaFunctions[0]->name, Eq("tuna"));
    EXPECT_TRUE(tunaFunctions[0]->resolved);
    EXPECT_THAT(tunaFunctions[0]->address, Eq(0));
    Word tunaWords[] = { ILOAD, 0, 1, IBRANCHR, 1, };
    EXPECT_THAT(tunaFunctions[0]->words, ElementsAreArray(tunaWords, sizeof(tunaWords)/sizeof(tunaWords[0])));
}

// Section - negative tests

TEST(SymbolTablePopulatorTest, InvalidInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnArg(Parser::Handler::LITERAL, "1", 0);
    stp.OnArg(Parser::Handler::REGISTER, "0", 0);
    stp.OnInstruction("INVALID INSTRUCTION", 5);
    stp.OnEnd();

    EXPECT_THAT(stp.Errors(), ElementsAre(Error("Invalid instruction 'INVALID INSTRUCTION' found on line 5")));
}

TEST(SymbolTablePopulatorTest, InvalidRegister) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
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
    stp.OnEnd();

    Error e1("Invalid register 'invalid register' found on line 6");
    Error e2("Invalid register '-1' found on line 7");
    EXPECT_THAT(stp.Errors(), ElementsAre(e1, e2));
}

TEST(SymbolTablePopulatorTest, InstructionBeforeFunction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnInstruction("LOAD", 7);
    stp.OnEnd();

    EXPECT_THAT(stp.Errors(),
            ElementsAre(Error("Received instruction 'LOAD' before function started on line 7")));
}

TEST(SymbolTablePopulatorTest, ArgBeforeFunction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnArg(Parser::Handler::LITERAL, "1", 8);
    stp.OnEnd();

    EXPECT_THAT(stp.Errors(),
            ElementsAre(Error("Received arg '1' before function started on line 8")));
}

TEST(SymbolTablePopulatorTest, ArgBeforeInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 9);
    stp.OnArg(Parser::Handler::LITERAL, "2", 10);
    stp.OnEnd();

    EXPECT_THAT(stp.Errors(),
            ElementsAre(Error("Received arg '2' before instruction on line 10")));
}

TEST(SymbolTablePopulatorTest, PartialInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 1);
    stp.OnInstruction("LOAD", 2);
    stp.OnArg(Parser::Handler::LITERAL, "2", 2);
    stp.OnArg(Parser::Handler::REGISTER, "2", 2);
    stp.OnInstruction("BRANCH", 3);
    stp.OnInstruction("STORE", 4);
    stp.OnArg(Parser::Handler::REGISTER, "0", 4);
    stp.OnArg(Parser::Handler::REFERENCE, "64", 4);

    EXPECT_THAT(stp.Errors(),
            ElementsAre(Error("Partial statement on line 3: expected 1 args but got 0")));
}

TEST(SymbolTablePopulatorTest, PartialLastFunctionInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();

    stp.OnFunction("tuna", 1);
    stp.OnInstruction("LOAD", 2);
    stp.OnArg(Parser::Handler::LITERAL, "2", 2);
    stp.OnArg(Parser::Handler::REGISTER, "2", 2);
    stp.OnInstruction("BRANCH", 3);
    stp.OnArg(Parser::Handler::LITERAL, "64", 6);
    stp.OnInstruction("STORE", 4);
    stp.OnArg(Parser::Handler::REGISTER, "0", 4);

    stp.OnFunction("fish", 5);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::LITERAL, "2", 6);
    stp.OnArg(Parser::Handler::REGISTER, "2", 6);
    stp.OnInstruction("BRANCH", 7);
    stp.OnArg(Parser::Handler::LITERAL, "64", 7);

    stp.OnEnd();

    EXPECT_THAT(stp.Errors(), SizeIs(1));
    EXPECT_THAT(stp.Errors()[0].S(), Eq("Partial statement on line 4: expected 2 args but got 1"));
}

TEST(SymbolTablePopulatorTest, PartialLastOverallInstruction) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 3);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::LITERAL, "0", 6);
    stp.OnArg(Parser::Handler::REGISTER, "2", 6);
    stp.OnInstruction("STORE", 9);
    stp.OnArg(Parser::Handler::LITERAL, "0", 9);
    stp.OnEnd();

    ASSERT_THAT(stp.Errors(), SizeIs(1));
    EXPECT_THAT(stp.Errors()[0].S(),
            Eq("Partial statement on last line: expected 2 args but got 1"));
}

TEST(SymbolTablePopulatorTest, InvalidArg) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 3);
    stp.OnInstruction("BRANCH", 6);
    stp.OnArg(Parser::Handler::REFERENCE, "0", 6);

    ASSERT_THAT(stp.Errors(), SizeIs(1));
    EXPECT_THAT(stp.Errors()[0].S(),
            Eq("Could not match BRANCH instruction with type 1"));
}

TEST(SymbolTablePopulatorTest, TooManyArgs) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnStart();
    stp.OnFunction("tuna", 3);
    stp.OnInstruction("LOAD", 6);
    stp.OnArg(Parser::Handler::REGISTER, "0", 6);
    stp.OnArg(Parser::Handler::REGISTER, "1", 6);
    stp.OnArg(Parser::Handler::REGISTER, "2", 6);

    ASSERT_THAT(stp.Errors(), SizeIs(1));
    EXPECT_THAT(stp.Errors()[0].S(),
            Eq("Too many args in statement on line 6: expected 2 args but got 3"));
}

// We test all instructions that are in cpu.h
