#include <sstream>

#include "gtest/gtest.h"

#include "src/assembler/writer.h"
#include "log.h"

TEST(WriteTest, A) {
    TestLog log;
    Writer w(&log);
    std::ostringstream os;
    Parser::Result result;
    const char *error = w.Write(result, os);
    EXPECT_EQ(error, nullptr);
}
