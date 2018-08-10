#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/cpu.h"
#include "src/program.h"
#include "src/system.h"

using testing::ElementsAre;

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
