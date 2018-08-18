#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/cpu.h"
#include "src/program.h"
#include "src/system.h"

using testing::ElementsAre;
using testing::Ne;

TEST(ProgramTest, AddWords) {
    Program p;

    const std::vector<Word> words = {1, 2, 3, 4, 5};
    p.AddWords(3, words);

    const std::vector<Word> more = {-1, 0, 1};
    p.AddWords(0, more);

    p.AddWord(4, 10);
    p.AddWord(9, 20);

    EXPECT_THAT(p.Words(), ElementsAre(-1, 0, 1, 1, 10, 3, 4, 5, 0, 20));
}

TEST(ProgramTest, Equals) {
    Program p;
    p.SetEntryAddress(5);

    const std::vector<Word> words = {1, 2, 3, 4, 5};
    p.AddWords(3, words);

    const std::vector<Word> more = {1, 0, 1};
    p.AddWords(0, more);

    p.AddWord(4, 10);
    p.AddWord(9, 20);
    p.AddWord(10, IEXIT);

    Program other(1, 0, 1, 1, 10, 3, 4, 5, 0, 20, IEXIT);
    other.SetEntryAddress(5);

    ASSERT_EQ(p, other);
}

TEST(ProgramTest, NotEquals) {
    Program p0(1, 2, 3);
    p0.SetEntryAddress(5);
    Program p1(1, 2, 3);
    Program p2(1, 2, 4);
    Program p3(1, 2, 4, 6);
    EXPECT_THAT(p0, Ne(p1));
    EXPECT_THAT(p0, Ne(p2));
    EXPECT_THAT(p0, Ne(p3));
    EXPECT_THAT(p1, Ne(p2));
    EXPECT_THAT(p1, Ne(p3));
    EXPECT_THAT(p2, Ne(p3));
}
