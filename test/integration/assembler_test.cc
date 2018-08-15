#include <fstream>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/asm/assembler.h"
#include "src/util/log.h"
#include "src/util/error.h"

class AssemblerTest : public testing::Test {
    protected:
        Assembler asm_;
};

TEST_F(AssemblerTest, Bad) {
    std::ifstream ifs("./test/integration/fixtures/bad.asm");
    ASSERT_TRUE(ifs);

    std::ostringstream oss;

    Error error = asm_.Run(new StdoutLog(), ifs, oss);
    ASSERT_TRUE(error);
    // TODO: EXPECT_THAT(error.S(), testing::Eq(""));
}

TEST_F(AssemblerTest, Good) {
    std::ifstream ifs("./test/integration/fixtures/good.asm");
    ASSERT_TRUE(ifs);

    std::ostringstream oss;

    Error error = asm_.Run(new StdoutLog(), ifs, oss);
    ASSERT_FALSE(error);
}
