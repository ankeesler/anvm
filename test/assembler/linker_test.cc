#include <sstream>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/assembler/linker.h"
#include "src/assembler/symbol_table.h"
#include "src/cpu.h"
#include "src/program.h"
#include "src/util/log.h"

using testing::ElementsAreArray;
using testing::Eq;

TEST(LinkerTest, Happy) {
    Log *log = new StdoutLog();
    SymbolTable st(log);

    Symbol tunaSymbol("tuna", true, 0, Symbol::FUNCTION);
    tunaSymbol.words.push_back(10);
    tunaSymbol.words.push_back(20);
    tunaSymbol.words.push_back(30);
    st.AddSymbol(tunaSymbol);

    st.AddSymbol(Symbol("fish", false, 1, Symbol::FUNCTION));

    Symbol marlinSymbol("marlin", true, 3, Symbol::FUNCTION);
    marlinSymbol.words.push_back(13);
    marlinSymbol.words.push_back(23);
    marlinSymbol.words.push_back(33);
    st.AddSymbol(marlinSymbol);

    Symbol fishSymbol("fish", true, 6, Symbol::FUNCTION);
    fishSymbol.words.push_back(16);
    fishSymbol.words.push_back(26);
    fishSymbol.words.push_back(36);
    st.AddSymbol(fishSymbol);

    Linker l(log);
    Program p;
    Error error = l.Link(st, &p);
    ASSERT_FALSE(error);
    EXPECT_EQ(p.EntryAddress(), 0);

    const Word expectedWords[] = {
        10, 6, 30,
        13, 23, 33,
        16, 26, 36,
    };
    EXPECT_THAT(p.Words(), ElementsAreArray(expectedWords, sizeof(expectedWords)/sizeof(expectedWords[0])));
}

TEST(LinkerTest, UnresolvedSymbols) {
    Log *log = new StdoutLog();
    SymbolTable st(log);

    Symbol tunaSymbol("tuna", true, 0, Symbol::FUNCTION);
    tunaSymbol.words.push_back(10);
    tunaSymbol.words.push_back(20);
    tunaSymbol.words.push_back(30);
    st.AddSymbol(tunaSymbol);

    st.AddSymbol(Symbol("fish", false, 1, Symbol::FUNCTION));

    Symbol marlinSymbol("marlin", true, 3, Symbol::FUNCTION);
    marlinSymbol.words.push_back(13);
    marlinSymbol.words.push_back(23);
    marlinSymbol.words.push_back(33);
    st.AddSymbol(marlinSymbol);

    st.AddSymbol(Symbol("fish", false, 4, Symbol::FUNCTION));
    st.AddSymbol(Symbol("bar", false, 5, Symbol::FUNCTION));

    Linker l(log);
    Program p;
    Error error = l.Link(st, &p);
    EXPECT_TRUE(error);
    EXPECT_THAT(error.S(), Eq("Unresolved symbol fish at addresses 0x00000001 0x00000004 \nUnresolved symbol bar at addresses 0x00000005 \n"));
}

TEST(LinkerTest, DuplicateSymbols) {
    Log *log = new StdoutLog();
    SymbolTable st(log);

    Symbol tunaSymbol("tuna", true, 0, Symbol::FUNCTION);
    tunaSymbol.words.push_back(10);
    tunaSymbol.words.push_back(20);
    tunaSymbol.words.push_back(30);
    st.AddSymbol(tunaSymbol);

    st.AddSymbol(Symbol("fish", false, 1, Symbol::FUNCTION));

    Symbol marlinSymbol("marlin", true, 3, Symbol::FUNCTION);
    marlinSymbol.words.push_back(13);
    marlinSymbol.words.push_back(23);
    marlinSymbol.words.push_back(33);
    st.AddSymbol(marlinSymbol);

    st.AddSymbol(Symbol("blah", true, 30, Symbol::FUNCTION));
    st.AddSymbol(Symbol("tuna", true, 4, Symbol::FUNCTION));
    st.AddSymbol(Symbol("fish", true, 10, Symbol::FUNCTION));
    st.AddSymbol(Symbol("blah", true, 15, Symbol::FUNCTION));

    Linker l(log);
    Program p;
    Error error = l.Link(st, &p);
    EXPECT_TRUE(error);
    EXPECT_THAT(error.S(), Eq("Duplicate symbol tuna at address 4\nDuplicate symbol blah at address 15\n"));
}

TEST(LinkerTest, MainFunctionEntryPoint) {
    Log *log = new StdoutLog();
    SymbolTable st(log);

    Symbol tunaSymbol("tuna", true, 0, Symbol::FUNCTION);
    tunaSymbol.words.push_back(10);
    tunaSymbol.words.push_back(20);
    tunaSymbol.words.push_back(30);
    st.AddSymbol(tunaSymbol);

    st.AddSymbol(Symbol("fish", false, 1, Symbol::FUNCTION));

    Symbol marlinSymbol("main", true, 3, Symbol::FUNCTION);
    marlinSymbol.words.push_back(13);
    marlinSymbol.words.push_back(23);
    marlinSymbol.words.push_back(33);
    st.AddSymbol(marlinSymbol);

    Symbol fishSymbol("fish", true, 6, Symbol::FUNCTION);
    fishSymbol.words.push_back(16);
    fishSymbol.words.push_back(26);
    fishSymbol.words.push_back(36);
    st.AddSymbol(fishSymbol);

    Linker l(log);
    Program p;
    Error error = l.Link(st, &p);
    ASSERT_FALSE(error);
    EXPECT_EQ(p.EntryAddress(), 3);

    const Word expectedWords[] = {
        10, 6, 30,
        13, 23, 33,
        16, 26, 36,
    };
    EXPECT_THAT(p.Words(), ElementsAreArray(expectedWords, sizeof(expectedWords)/sizeof(expectedWords[0])));
}
