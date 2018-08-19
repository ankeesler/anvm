#include <fstream>
#include <sstream>
#include <future>
#include <thread>
#include <chrono>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/cpu.h"
#include "src/system.h"
#include "src/program.h"
#include "src/asm/assembler.h"
#include "src/util/log.h"
#include "src/util/error.h"
#include "src/util/program_reader.h"

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::ContainerEq;
using ::testing::Eq;

class AssemblerTest : public testing::Test {
    protected:
        Assembler asm_;
};

static Word RunProgram(const Program& p, CPU *cpu) {
    System s(cpu);
    std::promise<Word> promise;
    std::thread thread([&]() { promise.set_value(s.Run(p)); });
    thread.detach();
    std::future<Word> fut = promise.get_future();
    std::future_status fut_status = fut.wait_for(std::chrono::seconds(3));
    if (fut_status == std::future_status::timeout) {
        s.Halt();
        return CPU::IEXIT;
    } else {
        return fut.get();
    }
}

TEST_F(AssemblerTest, Bad) {
    std::ifstream ifs("./test/integration/fixtures/bad.asm");
    ASSERT_TRUE(ifs);

    std::ostringstream oss;
    Error error = asm_.Run(new StdoutLog(), ifs, oss);
    ASSERT_TRUE(error);
    ASSERT_THAT(error.S(), testing::Eq("ERROR(s) during parsing:\n- Invalid register 'p' found on line 3\n- Partial statement on line 4: expected 2 args but got 1"));
}

TEST_F(AssemblerTest, Good) {
    std::ifstream ifs("./test/integration/fixtures/good.asm");
    ASSERT_TRUE(ifs);

    std::stringstream ss;

    Error error1 = asm_.Run(new StdoutLog(), ifs, ss);
    ASSERT_FALSE(error1);

    Program p;
    ProgramReader pr(new StdoutLog());
    Error error2 = pr.Read(ss, &p);
    ASSERT_FALSE(error2);
    
    const Word words[] = {
            // op1
            CPU::CPU::ILOADR, 1, 2,
            CPU::CPU::ILOAD, 1, 1,
            CPU::IADD,
            CPU::CPU::ILOADR, 0, 1,
            CPU::IMULTIPLY,
            CPU::ISTORE, 2, 64,
            CPU::CPU::ILOADM, 64, 1,
            CPU::IDIVIDE,
            CPU::CPU::ILOAD, 3, 1,
            CPU::ISUBTRACT,
            CPU::IBRANCHR, 2,
            
            // main
            CPU::CPU::ILOADR, CPU::PC, 0,
            CPU::CPU::ILOAD, 15, 1,
            CPU::IADD,
            CPU::CPU::ILOADR, 0, 1,
            CPU::CPU::ILOAD, 5, 0,
            CPU::IBRANCHX, 0,
            CPU::ISTORER, 0, 64,
            CPU::IEXIT
    };
    ASSERT_THAT(p.Words(), ElementsAreArray(words, sizeof(words)/sizeof(words[0])));
    ASSERT_THAT(p.EntryAddress(), Eq(24));

    CPU cpu(4, 68);
    Word status = RunProgram(p, &cpu);
    ASSERT_EQ(status, 0);

    Word value = cpu.ReadMem(64);
    ASSERT_EQ(value, 15);
}
