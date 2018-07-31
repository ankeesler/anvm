#include <ostream>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/program.h"
#include "src/util/program_writer.h"
#include "src/util/log.h"
#include "src/util/error.h"

TEST(ProgramWriterTest, Basic) {
    StdoutLog log;
    ProgramWriter w(&log);

    Program p(
            IBRANCHX, 8,
            IADD,
            ILOADM, 48, 1,
            IBRANCHR, 1,
            IEXIT
            );
    std::ostringstream os;
    Error e = w.Write(p, os);
    EXPECT_FALSE(e);
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
}
