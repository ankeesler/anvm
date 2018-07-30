#include <sstream>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/assembler/linker.h"
#include "src/assembler/parser.h"
#include "src/cpu.h"
#include "src/util/log.h"

class BasicLinkTest : public testing::Test {
    public:
        BasicLinkTest() {
            StdoutLog parserLog;
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

            StdoutLog linkerLog;
            Linker l(&linkerLog);
            const char *error = l.Link(result, &program_);
            EXPECT_EQ(error, nullptr);
        }

    protected:
        Program program_;
};

TEST_F(BasicLinkTest, Instructions) {
    EXPECT_EQ(program_.Words().size(), 21);

    const Word expectedOutput[] = {
        ILOAD, 1, 2,
        ILOADM, 32, 3,
        ISTORE, 1, 2,
        IADD,
        ILOAD, 1, 0,
        ILOADR, 2, 1,
        IMULTIPLY,
        ILOAD, 0, SP,
        IEXIT,
    };
    EXPECT_THAT(program_.Words(), testing::ElementsAreArray(expectedOutput, 21));
    (void)expectedOutput;
}
