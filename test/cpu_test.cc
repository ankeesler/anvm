#include <thread>
#include <future>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "program_library.h"
#include "src/cpu.h"
#include "src/program.h"
#include "src/system.h"

using testing::ElementsAre;

class SingleCPUTest : public testing::Test {
    public:
        SingleCPUTest() {
            cpu_ = new CPU(4, 64);
            system_ = new System(cpu_);
        }
        ~SingleCPUTest() {
            delete system_;
            delete cpu_;
        }

    protected:
        CPU *cpu_;
        System *system_;
};

TEST_F(SingleCPUTest, Add) {
    Program p(
            CPU::CPU::ILOAD, 1, 0,
            CPU::CPU::ILOAD, 2, 1,
            CPU::IADD,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Subtract) {
    Program p(
            CPU::CPU::ILOAD, 1, 0,
            CPU::CPU::ILOAD, 2, 1,
            CPU::ISUBTRACT,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(-1, r);
}

TEST_F(SingleCPUTest, Multiply) {
    Program p(
            CPU::CPU::ILOAD, 3, 0,
            CPU::CPU::ILOAD, 2, 1,
            CPU::IMULTIPLY,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(6, r);
}

TEST_F(SingleCPUTest, Divide) {
    Program p(
            CPU::CPU::ILOAD, -4, 0,
            CPU::CPU::ILOAD, 2, 1,
            CPU::IDIVIDE,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(-2, r);
}

TEST_F(SingleCPUTest, DivideBy0) {
    Program p(
            CPU::CPU::ILOAD, -4, 0,
            CPU::CPU::ILOAD, 0, 1,
            CPU::IDIVIDE,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), CPU::DIVZERO);
}

TEST_F(SingleCPUTest, Mod) {
    Program p(
            CPU::CPU::ILOAD, 150, 0,
            CPU::CPU::ILOAD, 147, 1,
            CPU::IMOD,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Branch) {
    Program p(
            CPU::CPU::ILOAD, 7, 0,
            CPU::IBRANCH,
            0, 0, 0, // whatever instructions
            CPU::CPU::ILOAD, 2, 0,
            CPU::CPU::ILOAD, 3, 1,
            CPU::IADD, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
}

TEST_F(SingleCPUTest, BadAccess) {
    Program p(
            CPU::CPU::ILOAD, cpu_->MemSize() + 1, 0,
            CPU::IBRANCH,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), CPU::BADACCESS);
}

TEST_F(SingleCPUTest, CmpEq) {
    Program p1(
            CPU::CPU::ILOAD, 7, 0,
            CPU::CPU::ILOAD, 6, 1,
            CPU::ICMPEQ, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p2(
            CPU::CPU::ILOAD, 9, 0,
            CPU::CPU::ILOAD, 9, 1,
            CPU::ICMPEQ, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(1, r);
}

TEST_F(SingleCPUTest, CmpLt) {
    Program p1(
            CPU::CPU::ILOAD, 7, 0,
            CPU::CPU::ILOAD, 6, 1,
            CPU::ICMPLT, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p2(
            CPU::CPU::ILOAD, 9, 0,
            CPU::CPU::ILOAD, 9, 1,
            CPU::ICMPLT, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p3(
            CPU::CPU::ILOAD, 11, 0,
            CPU::CPU::ILOAD, 12, 1,
            CPU::ICMPLT, 
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p3), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(1, r);
}

TEST_F(SingleCPUTest, BranchIf) {
    Program p1(
            CPU::CPU::ILOAD, 2, 0,
            CPU::CPU::ILOAD, 14, 1,
            CPU::IBRANCHIF,
            CPU::CPU::ILOAD, 222, 1,
            CPU::CPU::ILOAD, 17, 0,
            CPU::IBRANCH,
            CPU::CPU::ILOAD, 333, 1,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(1);
    EXPECT_EQ(333, r);

    Program p2(
            CPU::CPU::ILOAD, 0, 0,
            CPU::CPU::ILOAD, 7, 1,
            CPU::IBRANCHIF,
            CPU::CPU::ILOAD, 222, 1,
            CPU::CPU::ILOAD, 17, 0,
            CPU::IBRANCH,
            CPU::CPU::ILOAD, 333, 1,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(1);
    EXPECT_EQ(222, r);
}

TEST_F(SingleCPUTest, Decrement) {
    Program p(
            CPU::CPU::ILOAD, 1000, 0,
            CPU::IDEC,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(999, r);
}

TEST_F(SingleCPUTest, Increment) {
    Program p(
            CPU::CPU::ILOAD, 6999, 0,
            CPU::IINC,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(7000, r);
}

TEST_F(SingleCPUTest, BadRegister) {
    Program p1(
            CPU::CPU::ILOAD, 1, 100,
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p1), CPU::BADREGISTER);

    Program p2(
            CPU::CPU::ILOADR, 1, 100, 
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p2), CPU::BADREGISTER);

    Program p3(
            CPU::CPU::ILOADR, 100, 1, 
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p3), CPU::BADREGISTER);
}

TEST_F(SingleCPUTest, ForLoop) {
    // int a = 1;
    // for (int i = 0; i < 3; i++) {
    //     a *= 3;
    // }
    Program p(
            CPU::CPU::ILOAD, 1, 2, // a = 1
            CPU::CPU::ILOAD, 0, 3, // i = 0
            CPU::CPU::ILOAD, 3, 1,

            CPU::CPU::ILOAD, 30, 0, // goto loop comparison
            CPU::IBRANCH,

            // multiplication:
            CPU::CPU::ILOADR, 2, 0, // gr0 = a
            CPU::CPU::ILOAD, 3, 1,  // gr1 = 3
            CPU::IMULTIPLY,    // gr0 = gr0(a) * gr1(3) 
            CPU::CPU::ILOADR, 0, 2, // a = gr0

            // increment i:
            CPU::CPU::ILOADR, 3, 0, // gr0 = 1
            CPU::IINC,         // gr0(i) ++
            CPU::CPU::ILOADR, 0, 3, // i = gr0(i)

            // loop comparison:
            CPU::CPU::ILOADR, 3, 0, // gr0 = i
            CPU::CPU::ILOAD, 3, 1,  // gr1 = 3
            CPU::ICMPLT,       // if i < 3,
            CPU::CPU::ILOAD, 13, 1, // goto multiplication
            CPU::IBRANCHIF,

            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(2);
    EXPECT_EQ(27, r);
}

TEST_F(SingleCPUTest, NotEnoughMemory) {
    Program p(
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p), CPU::BADACCESS);
}

TEST_F(SingleCPUTest, BranchX) {
    Program p(
            CPU::CPU::ILOAD, 1000, 0,
            CPU::IBRANCHX, 6,
            CPU::IINC,
            CPU::IDEC,
            CPU::IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(999, r);
}

TEST_F(SingleCPUTest, FunctionCall) {
    // int add(int a, int b) {
    //     return a + b;
    // }
    //
    // int foo() {
    //     int a = add(1, 2);
    //     int b = add(3, 4);
    //     return a * b;
    // }
    Program p(
            CPU::IBRANCHX, 8,

            // add(r0, r1) -> r0:
            CPU::IADD,
            CPU::CPU::ILOADM, 48, 1,
            CPU::IBRANCHR, 1,

            CPU::CPU::ILOAD, 1, 0,
            CPU::CPU::ILOAD, 2, 1,
            CPU::ISTORE, 19, 48, // push return address
            CPU::IBRANCHX, 2,    // call add
            CPU::CPU::ILOADR, 0, 2,   // r2 = a
            CPU::CPU::ILOAD, 3, 0,
            CPU::CPU::ILOAD, 4, 1,
            CPU::ISTORE, 33, 48, // push return address
            CPU::IBRANCHX, 2,    // call add
            CPU::CPU::ILOADR, 0, 3,   // r3 = b
            CPU::CPU::ILOADR, 2, 0,   // r0 = a
            CPU::CPU::ILOADR, 3, 1,   // r1 = b
            CPU::IMULTIPLY,

            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(21, r);
}

TEST_F(SingleCPUTest, Store) {
    Program p(
            CPU::ISTORE, 5, 32,
            CPU::CPU::ILOADM, 32, 0,
            CPU::ISTORER, 0, 16,
            CPU::IEXIT
            );

    ASSERT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
    Word m = cpu_->ReadMem(16);
    EXPECT_EQ(5, m);
}

TEST_F(SingleCPUTest, StackPointer) {
    Program p(
            CPU::CPU::ILOAD, 5, CPU::SP,
            CPU::CPU::ILOADR, CPU::SP, 0,
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
}

TEST_F(SingleCPUTest, ProgramCounter) {
    Program p(
            CPU::CPU::ILOAD, 5, 0,
            CPU::CPU::ILOADR, CPU::PC, 1,
            CPU::CPU::ILOAD, 8, 0,
            CPU::IADD,
            CPU::IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(11, r);
}

TEST_F(SingleCPUTest, EntryAddress) {
    Program p(
            CPU::CPU::ILOAD, 5, 0,
            CPU::CPU::ILOAD, 3, 1,
            CPU::IADD,
            CPU::IEXIT
            );
    p.SetEntryAddress(3);

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Halt) {
    const Program& p = ProgramLibrary::Instance().InfiniteLoop();

    std::promise<Word> prom;
    std::thread thread([&]() { prom.set_value(system_->Run(p)); });
    thread.detach();
    std::future<Word> fut = prom.get_future();
    std::future_status fut_status = fut.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(fut_status, std::future_status::timeout);
    system_->Halt();
    fut_status = fut.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(fut_status, std::future_status::ready);
    Word status = fut.get();
    EXPECT_EQ(status, CPU::HALTED);
}
