#include <istream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/program.h"
#include "src/util/program_reader.h"
#include "src/util/log.h"

TEST(ProgramReaderTest, Basic) {
    const unsigned char programBytes[] = {
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
            CPU::IBRANCHX, 8,
            CPU::IADD,
            CPU::CPU::ILOADM, 48, 1,
            CPU::IBRANCHR, 1,
            CPU::IEXIT,
    };
    EXPECT_THAT(program.Words(), testing::ElementsAreArray(words, 9));
}

TEST(ProgramReaderTest, Underflow) {
    const unsigned char programBytes[] = {
        0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, CPU::IBRANCHX,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, CPU::IADD,
        0x00, 0x00,
    };
    const std::string programString((char *)programBytes, sizeof(programBytes));

    StdoutLog log;
    ProgramReader r(&log);

    Program program;
    std::istringstream is(programString);
    const Error error = r.Read(is, &program);
    ASSERT_TRUE(error);
    EXPECT_EQ(error.S(), "Could not read program text: Failed to read byte 2 of word 4");
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
    EXPECT_EQ(error.S(), "Could not read program entry address: Failed to read byte 1 of word 0");
}

TEST(ProgramReaderTest, File) {
    const unsigned char programBytes[] = {
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
    const std::string programString((char *)programBytes, sizeof(programBytes));
    std::ofstream ofs("program.out");
    EXPECT_TRUE(ofs.write(programString.c_str(), programString.size()));
    ofs.close();

    StdoutLog log;
    ProgramReader r(&log);

    Program program;
    std::ifstream ifs("program.out");

    const Error error = r.Read(ifs, &program);
    ASSERT_FALSE(error);

    EXPECT_EQ(program.Words().size(), 9);

    EXPECT_THAT(program.EntryAddress(), testing::Eq(5));

    const Word words[] = {
            CPU::IBRANCHX, 8,
            CPU::IADD,
            CPU::CPU::ILOADM, 48, 1,
            CPU::IBRANCHR, 1,
            CPU::IEXIT,
    };
    EXPECT_THAT(program.Words(), testing::ElementsAreArray(words, 9));
}
