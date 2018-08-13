#include <istream>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/program.h"
#include "src/util/program_reader.h"
#include "src/util/log.h"

TEST(ProgramReaderTest, Basic) {
    const unsigned char programBytes[] = {
        0x00, 0x00, 0x00, 0x05,
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

    StdoutLog log;
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);
    const Error error = r.Read(is, &program);
    ASSERT_FALSE(error);
    EXPECT_EQ(program.Words().size(), 9);

    EXPECT_THAT(program.EntryAddress(), testing::Eq(5));

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
        0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, IBRANCHX,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, IADD,
        0x00, 0x00,
    };
    const std::string programString((char *)programBytes, sizeof(programBytes));

    StdoutLog log;
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);
    const Error error = r.Read(is, &program);
    ASSERT_TRUE(error);
    EXPECT_EQ(error.S(), "Program underflow at word 4; got 2 bytes but wanted 4");
}

TEST(ProgramReaderTest, AlmostEmpty) {
    const unsigned char programBytes[] = {
        0x00,
    };
    const std::string programString((char *)programBytes, sizeof(programBytes));

    StdoutLog log;
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);

    const Error error = r.Read(is, &program);
    ASSERT_TRUE(error);
    EXPECT_EQ(error.S(), "Program underflow at word 0; got 1 bytes but wanted 4");
}
