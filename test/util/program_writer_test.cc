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
            CPU::IBRANCHX, 8,
            CPU::IADD,
            CPU::CPU::ILOADM, 48, 1,
            CPU::IBRANCHR, 1,
            CPU::IEXIT
            );
    p.SetEntryAddress(5);
    std::ostringstream os;
    Error e = w.Write(p, os);
    ASSERT_FALSE(e);
    const std::string&& bytes = os.str();

    const unsigned char expectedOutput[] = {
        0x00, 0x00, 0x00, 0x05,
        0x00, 0x00, 0x00, CPU::IBRANCHX,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, CPU::IADD,
        0x00, 0x00, 0x00, CPU::CPU::ILOADM,
        0x00, 0x00, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, CPU::IBRANCHR,
        0x00, 0x00, 0x00, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,
    };
    EXPECT_THAT(bytes, testing::ElementsAreArray(expectedOutput, sizeof(expectedOutput)));
}
