#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/assembler/symbol_table.h"
#include "src/util/log.h"

using testing::ElementsAre;
using testing::SizeIs;
using testing::IsEmpty;
using testing::Eq;

TEST(SymbolTableTest, Basic) {
    SymbolTable::Function tunaFcn;
    tunaFcn.SetName("tuna");
    tunaFcn.AddWord(1);
    tunaFcn.AddWord(2);
    tunaFcn.AddWord(3);

    SymbolTable::Function marlin1Fcn;
    marlin1Fcn.SetName("marlin");
    marlin1Fcn.AddWord(20);
    marlin1Fcn.AddWord(30);

    SymbolTable::Function marlin2Fcn;
    marlin2Fcn.SetName("marlin");
    marlin2Fcn.AddWord(40);
    marlin2Fcn.AddWord(50);

    SymbolTable st(new StdoutLog);
    st.AddFunction(tunaFcn);
    st.AddFunction(marlin1Fcn);
    st.AddFunction(marlin2Fcn);

    const std::vector<std::string>& names = st.FunctionNames();
    EXPECT_THAT(names, ElementsAre("tuna", "marlin"));

    std::vector<SymbolTable::Function> functions = st.FindFunctions("tuna");
    EXPECT_THAT(functions, SizeIs(1));
    EXPECT_THAT(functions[0], Eq(tunaFcn));

    functions = st.FindFunctions("marlin");
    EXPECT_THAT(functions, SizeIs(2));
    EXPECT_THAT(functions[0], Eq(marlin1Fcn));
    EXPECT_THAT(functions[1], Eq(marlin2Fcn));
}
