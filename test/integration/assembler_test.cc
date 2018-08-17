#include <fstream>
#include <sstream>
#include <future>
#include <thread>
#include <chrono>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/cpu.h"
#include "src/asm/assembler.h"
#include "src/util/log.h"
#include "src/util/error.h"
#include "src/util/program_reader.h"

class AssemblerTest : public testing::Test {
    protected:
        Assembler asm_;
};

static void RunProgram(const Program& p, std::promise<Word> *promise) {
    promise->set_value(5);
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

    Program p(
            ILOAD, 1, 0,
            ILOAD, 2, 1,
            IADD,
            IEXIT
           );
    std::promise<Word> promise;
    std::thread thread(RunProgram, std::ref(p), &promise);
    std::future<Word> fut = promise.get_future();
    std::future_status fut_status = fut.wait_for(std::chrono::seconds(3));
    if (fut_status == std::future_status::timeout) {
        std::cout << "timeout!" << std::endl;
        ASSERT_TRUE(false);
    } else {
        std::cout << "it finished!" << std::endl;
        ASSERT_TRUE(true);
    }
}
