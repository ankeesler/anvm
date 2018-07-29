#include <sstream>
#include <vector>

#include "gtest/gtest.h"

#include "src/assembler/writer.h"
#include "src/assembler/parser.h"
#include "log.h"

class BasicWriteTest : public testing::Test {
    public:
        BasicWriteTest() {
            TestLog parserLog;
            Parser p(&parserLog);
            std::string text =
                "tuna:\n"
                "  LOAD 1 2\n"
                "  LOAD @32 3\n"
                "\n"
                "fish:\n"
                "  STORE 1 @2\n"
                "  ADD\n"
                "\n"
                "marlin:\n"
                "  LOAD 1 %r0\n"
                "  LOAD %r2 1\n"
                "  MULTIPLY\n"
                "  LOAD 0 %rsp\n";
            std::stringstream is(text, std::ios_base::in);
            Parser::Result result = p.Parse(is);
            EXPECT_FALSE(result.Error(nullptr));

            TestLog writerLog(true);
            Writer w(&writerLog);
            const char *error = w.Write(result, &output_);
            EXPECT_EQ(error, nullptr);
        }

    protected:
        std::vector<Word> output_;
};

TEST_F(BasicWriteTest, Instructions) {
    EXPECT_EQ(output_.size(), 20);
}
