#include <istream>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/assembler/program_reader.h"
#include "src/program.h"
#include "log.h"

TEST(ProgramReaderTest, Basic) {
    const unsigned char programBytes[] = {
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
    const std::string programString((char *)programBytes, sizeof(programBytes));

    TestLog log(true);
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);
    const ProgramReader::Error& error = r.Read(is, &program);
    EXPECT_FALSE(error);
    EXPECT_EQ(program.Words().size(), 9);

    const Word words[] = {
            IBRANCHX, 8,
            IADD,
            ILOADM, 48, 1,
            IBRANCHR, 1,
            IEXIT,
    };
    EXPECT_THAT(program.Words(), testing::ElementsAreArray(words, 9));
}

TEST(ProgramReaderTest, Underflow) {
    const unsigned char programBytes[] = {
        0x00, 0x00, 0x00, IBRANCHX,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, IADD,
        0x00, 0x00,
    };
    const std::string programString((char *)programBytes, sizeof(programBytes));

    TestLog log(true);
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);
    const ProgramReader::Error& error = r.Read(is, &program);
    EXPECT_TRUE(error);
    EXPECT_EQ(error.Get(), "ERROR: program underflow at word 3; got 2 bytes but wanted 4");
}
