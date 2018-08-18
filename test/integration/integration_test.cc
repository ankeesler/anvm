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
        ProgramReader pr_;
};

static Word RunProgram(const Program& p, CPU *cpu) {
    std::promise<Word> promise;
    std::thread thread([&]() {
            System s(cpu);
            Word status = s.Run(p);
            promise.set_value(status);
    });
    thread.detach();
    std::future<Word> fut = promise.get_future();
    std::future_status fut_status = fut.wait_for(std::chrono::seconds(3));
    if (fut_status == std::future_status::timeout) {
        return IEXIT;
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
    // TODO: EXPECT_THAT(error.S(), testing::Eq(""));
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
            ILOAD, 1, 1,
            IADD,
            ILOADR, 0, 1,
            IMULTIPLY,
            ISTORE, 2, 64,
            ILOADM, 64, 1,
            IDIVIDE,
            ILOAD, 3, 0,
            ISUBTRACT,
            
            ILOADR, PC, 0,
            ILOAD, 15, 1,
            IADD,
            ILOADR, 0, 1,
            ILOAD, 5, 0,
            IBRANCHX, 0,
            ISTORER, 0, 64,
            IEXIT
    };
    ASSERT_THAT(p.Words(), ElementsAreArray(words, sizeof(words)/sizeof(words[0])));
    ASSERT_THAT(p.EntryAddress(), Eq(19));

    CPU cpu(4, 128);
    Word status = RunProgram(p, &cpu);
    ASSERT_EQ(status, 0);

    Word value = cpu.ReadMem(64);
    ASSERT_EQ(value, 15);
}
