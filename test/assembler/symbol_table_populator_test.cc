#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/cpu.h"
#include "src/assembler/symbol_table.h"
#include "src/assembler/symbol_table_populator.h"
#include "src/util/log.h"
#include "src/util/error.h"

using testing::SizeIs;
using testing::Eq;
using testing::ElementsAre;

TEST(SymbolTableTest, HappyPath) {
    Log *log = new StdoutLog();
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);

    stp.OnFunction("tuna", 0);
    stp.OnInstruction("LOAD", 0);
    stp.OnInstruction("BRANCH", 0);
    stp.OnFunction("fish", 0);
    stp.OnInstruction("STORE", 0);
    stp.OnInstruction("ADD", 0);

    const std::vector<SymbolTable::Function*>& tunaFunctions = st.FindFunctions("tuna");
    EXPECT_THAT(tunaFunctions, SizeIs(1));
    EXPECT_THAT(tunaFunctions[0]->Name(), Eq("tuna"));
    EXPECT_THAT(tunaFunctions[0]->Words(), SizeIs(2));
    EXPECT_THAT(tunaFunctions[0]->Words(), ElementsAre(ILOAD, IBRANCH));

    const std::vector<SymbolTable::Function*>& fishFunctions = st.FindFunctions("fish");
    EXPECT_THAT(fishFunctions, SizeIs(1));
    EXPECT_THAT(fishFunctions[0]->Name(), Eq("fish"));
    EXPECT_THAT(fishFunctions[0]->Words(), SizeIs(2));
    EXPECT_THAT(fishFunctions[0]->Words(), ElementsAre(ISTORE, IADD));
}

// On instruction, we add word to table
// On instruction that is invalid, we error
// On arg, we add word to table
// On arg that doesn't make sense for instruction, we error
// If we get a register that doesn't make sense, we throw up our hands
// After error, we stop adding stuff
// If we receive an instruction before we receive a function, we error
