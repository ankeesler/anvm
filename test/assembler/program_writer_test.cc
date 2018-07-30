#include <ostream>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/assembler/program_writer.h"
#include "src/program.h"
#include "log.h"

TEST(ProgramWriterTest, Foo) {
    Program p(
            IBRANCHX, 8,
            IADD,
            ILOADM, 48, 1,
            IBRANCHR, 1,
            IEXIT
            );

    TestLog log(true);
    ProgramWriter w(&log);
    std::ostringstream os;
    const char *error = w.Write(p, os);
    EXPECT_EQ(error, nullptr);
    const std::string&& bytes = os.str();
    EXPECT_EQ(bytes.size(), 36);

    const unsigned char expectedOutput[] = {
        0x00, 0x00, 0x00, IBRANCHX,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, IADD,
        0x00, 0x00, 0x00, ILOADM,
        0x00, 0x00, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, IBRANCHR,
        0x00, 0x00, 0x00, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,
    };
    EXPECT_THAT(bytes, testing::ElementsAreArray(expectedOutput, 36));
    (void)expectedOutput;
}
