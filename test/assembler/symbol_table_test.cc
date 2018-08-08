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
    Symbol tunaSymbol("tuna", true, 1, Symbol::FUNCTION);
    tunaSymbol.words.push_back(1);
    tunaSymbol.words.push_back(2);
    tunaSymbol.words.push_back(3);

    Symbol marlin1Symbol("marlin", true, 11, Symbol::FUNCTION);
    marlin1Symbol.words.push_back(20);
    marlin1Symbol.words.push_back(30);

    Symbol marlin2Symbol("marlin", false, 6, Symbol::FUNCTION);

    Symbol fishSymbol("fish", false, 9, Symbol::FUNCTION);

    SymbolTable st(new StdoutLog);
    Symbol *tunaSymbolPtr = st.AddSymbol(tunaSymbol);
    Symbol *marlin1SymbolPtr = st.AddSymbol(marlin1Symbol);
    Symbol *marlin2SymbolPtr = st.AddSymbol(marlin2Symbol);
    Symbol *fishSymbolPtr = st.AddSymbol(fishSymbol);

    EXPECT_THAT(tunaSymbolPtr->name, Eq("tuna"));
    EXPECT_THAT(marlin1SymbolPtr->name, Eq("marlin"));
    EXPECT_THAT(marlin2SymbolPtr->name, Eq("marlin"));
    EXPECT_THAT(fishSymbolPtr->name, Eq("fish"));

    const std::vector<std::string>& names = st.SymbolNames();
    EXPECT_THAT(names, ElementsAre("tuna", "marlin", "fish"));

    std::vector<Symbol*> symbols = st.Symbols("tuna");
    EXPECT_THAT(symbols, SizeIs(1));
    EXPECT_THAT(*symbols[0], Eq(tunaSymbol));
    EXPECT_THAT(*symbols[0], Eq(*tunaSymbolPtr));

    symbols = st.Symbols("marlin");
    EXPECT_THAT(symbols, SizeIs(2));
    EXPECT_THAT(*symbols[0], Eq(marlin1Symbol));
    EXPECT_THAT(*symbols[0], Eq(*marlin1SymbolPtr));
    EXPECT_THAT(*symbols[1], Eq(marlin2Symbol));
    EXPECT_THAT(*symbols[1], Eq(*marlin2SymbolPtr));

    symbols = st.Symbols("fish");
    EXPECT_THAT(symbols, SizeIs(1));
    EXPECT_THAT(*symbols[0], Eq(fishSymbol));
    EXPECT_THAT(*symbols[0], Eq(*fishSymbolPtr));

    symbols = st.Symbols("invalid symbol");
    EXPECT_THAT(symbols, SizeIs(0));
}
