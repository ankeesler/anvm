#include <thread>
#include <future>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
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
            ILOAD, 1, 0,
            ILOAD, 2, 1,
            IADD,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Subtract) {
    Program p(
            ILOAD, 1, 0,
            ILOAD, 2, 1,
            ISUBTRACT,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(-1, r);
}

TEST_F(SingleCPUTest, Multiply) {
    Program p(
            ILOAD, 3, 0,
            ILOAD, 2, 1,
            IMULTIPLY,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(6, r);
}

TEST_F(SingleCPUTest, Divide) {
    Program p(
            ILOAD, -4, 0,
            ILOAD, 2, 1,
            IDIVIDE,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(-2, r);
}

TEST_F(SingleCPUTest, DivideBy0) {
    Program p(
            ILOAD, -4, 0,
            ILOAD, 0, 1,
            IDIVIDE,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), STATUS_DIVZERO);
}

TEST_F(SingleCPUTest, Mod) {
    Program p(
            ILOAD, 150, 0,
            ILOAD, 147, 1,
            IMOD,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Branch) {
    Program p(
            ILOAD, 7, 0,
            IBRANCH,
            0, 0, 0, // whatever instructions
            ILOAD, 2, 0,
            ILOAD, 3, 1,
            IADD, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
}

TEST_F(SingleCPUTest, BadAccess) {
    Program p(
            ILOAD, cpu_->MemSize() + 1, 0,
            IBRANCH,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), STATUS_BADACCESS);
}

TEST_F(SingleCPUTest, CmpEq) {
    Program p1(
            ILOAD, 7, 0,
            ILOAD, 6, 1,
            ICMPEQ, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p2(
            ILOAD, 9, 0,
            ILOAD, 9, 1,
            ICMPEQ, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(1, r);
}

TEST_F(SingleCPUTest, CmpLt) {
    Program p1(
            ILOAD, 7, 0,
            ILOAD, 6, 1,
            ICMPLT, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p2(
            ILOAD, 9, 0,
            ILOAD, 9, 1,
            ICMPLT, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(0, r);

    Program p3(
            ILOAD, 11, 0,
            ILOAD, 12, 1,
            ICMPLT, 
            IEXIT
           );

    EXPECT_EQ(system_->Run(p3), 0);
    r = cpu_->ReadGR(0);
    EXPECT_EQ(1, r);
}

TEST_F(SingleCPUTest, BranchIf) {
    Program p1(
            ILOAD, 2, 0,
            ILOAD, 14, 1,
            IBRANCHIF,
            ILOAD, 222, 1,
            ILOAD, 17, 0,
            IBRANCH,
            ILOAD, 333, 1,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p1), 0);
    Word r = cpu_->ReadGR(1);
    EXPECT_EQ(333, r);

    Program p2(
            ILOAD, 0, 0,
            ILOAD, 7, 1,
            IBRANCHIF,
            ILOAD, 222, 1,
            ILOAD, 17, 0,
            IBRANCH,
            ILOAD, 333, 1,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p2), 0);
    r = cpu_->ReadGR(1);
    EXPECT_EQ(222, r);
}

TEST_F(SingleCPUTest, Decrement) {
    Program p(
            ILOAD, 1000, 0,
            IDEC,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(999, r);
}

TEST_F(SingleCPUTest, Increment) {
    Program p(
            ILOAD, 6999, 0,
            IINC,
            IEXIT
           );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(7000, r);
}

TEST_F(SingleCPUTest, BadRegister) {
    Program p1(
            ILOAD, 1, 100,
            IEXIT
            );

    EXPECT_EQ(system_->Run(p1), STATUS_BADREGISTER);

    Program p2(
            ILOADR, 1, 100, 
            IEXIT
            );

    EXPECT_EQ(system_->Run(p2), STATUS_BADREGISTER);

    Program p3(
            ILOADR, 100, 1, 
            IEXIT
            );

    EXPECT_EQ(system_->Run(p3), STATUS_BADREGISTER);
}

TEST_F(SingleCPUTest, ForLoop) {
    // int a = 1;
    // for (int i = 0; i < 3; i++) {
    //     a *= 3;
    // }
    Program p(
            ILOAD, 1, 2, // a = 1
            ILOAD, 0, 3, // i = 0
            ILOAD, 3, 1,

            ILOAD, 30, 0, // goto loop comparison
            IBRANCH,

            // multiplication:
            ILOADR, 2, 0, // gr0 = a
            ILOAD, 3, 1,  // gr1 = 3
            IMULTIPLY,    // gr0 = gr0(a) * gr1(3) 
            ILOADR, 0, 2, // a = gr0

            // increment i:
            ILOADR, 3, 0, // gr0 = 1
            IINC,         // gr0(i) ++
            ILOADR, 0, 3, // i = gr0(i)

            // loop comparison:
            ILOADR, 3, 0, // gr0 = i
            ILOAD, 3, 1,  // gr1 = 3
            ICMPLT,       // if i < 3,
            ILOAD, 13, 1, // goto multiplication
            IBRANCHIF,

            IEXIT
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
            IEXIT
            );

    EXPECT_EQ(system_->Run(p), STATUS_BADACCESS);
}

TEST_F(SingleCPUTest, BranchX) {
    Program p(
            ILOAD, 1000, 0,
            IBRANCHX, 6,
            IINC,
            IDEC,
            IEXIT
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
            IBRANCHX, 8,

            // add(r0, r1) -> r0:
            IADD,
            ILOADM, 48, 1,
            IBRANCHR, 1,

            ILOAD, 1, 0,
            ILOAD, 2, 1,
            ISTORE, 19, 48, // push return address
            IBRANCHX, 2,    // call add
            ILOADR, 0, 2,   // r2 = a
            ILOAD, 3, 0,
            ILOAD, 4, 1,
            ISTORE, 33, 48, // push return address
            IBRANCHX, 2,    // call add
            ILOADR, 0, 3,   // r3 = b
            ILOADR, 2, 0,   // r0 = a
            ILOADR, 3, 1,   // r1 = b
            IMULTIPLY,

            IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(21, r);
}

TEST_F(SingleCPUTest, Store) {
    Program p(
            ISTORE, 5, 32,
            ILOADM, 32, 0,
            IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
}

TEST_F(SingleCPUTest, StackPointer) {
    Program p(
            ILOAD, 5, SP,
            ILOADR, SP, 0,
            IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(5, r);
}

TEST_F(SingleCPUTest, ProgramCounter) {
    Program p(
            ILOAD, 5, 0,
            ILOADR, PC, 1,
            ILOAD, 8, 0,
            IADD,
            IEXIT
            );

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(11, r);
}

TEST_F(SingleCPUTest, EntryAddress) {
    Program p(
            ILOAD, 5, 0,
            ILOAD, 3, 1,
            IADD,
            IEXIT
            );
    p.SetEntryAddress(3);

    EXPECT_EQ(system_->Run(p), 0);
    Word r = cpu_->ReadGR(0);
    EXPECT_EQ(3, r);
}

TEST_F(SingleCPUTest, Halt) {
    // infinite loop...
    Program p(
            IBRANCHX, 0,
            IEXIT
            );

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
    EXPECT_EQ(status, STATUS_HALTED);
}
