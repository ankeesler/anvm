#ifndef ANVM_CPU_H_
#define ANVM_CPU_H_

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <atomic>

typedef int32_t Word;
#define MAX_WORD INT32_MAX

#define STATUS_UNKNOWNI ((Word)1)
#define STATUS_DIVZERO ((Word)2)
#define STATUS_BADACCESS ((Word)4)
#define STATUS_BADREGISTER ((Word)8)
#define STATUS_HALTED ((Word)16)
#define STATUS_FAILURE (STATUS_UNKNOWNI | STATUS_DIVZERO | STATUS_BADACCESS | STATUS_BADREGISTER | STATUS_HALTED)

#define SP ((Word)-2)
#define PC ((Word)-3)

#define IEXIT ((Word)-1) /* the program is done */

// Load into a register.
#define ILOAD ((Word)0x00) /* literal, reg; gr[reg] = literal */
#define ILOADR ((Word)0x01) /* regA, regB; gr[regB] = gr[regA] */
#define ILOADM ((Word)0x02) /* addr, reg; mem[addr] = gr[reg] */

// Store into memory.
#define ISTORE ((Word)0x10) /* literal, addr; mem[addr] = literal */
#define ISTORER ((Word)0x11) /* reg, addr; mem[addr] = gr[reg] */

// Branch to instruction.
#define IBRANCH ((Word)0x20) /* pc = gr[0] */
#define IBRANCHR ((Word)0x21) /* reg; pc = gr[reg] */
#define IBRANCHX ((Word)0x23) /* addr; pc = addr */
#define IBRANCHIF ((Word)0x24) /* if gr[0] != 0, pc = gr[1] */

// Perform math operations.
#define IADD ((Word)0xA0) /* gr[0] + gr[1] = gr[0] */
#define IMULTIPLY ((Word)0xA1) /* gr[0] * gr[1] = gr[0] */
#define ISUBTRACT ((Word)0xA2) /* gr[0] - gr[1] = gr[0] */
#define IDIVIDE ((Word)0xA3) /* gr[0] / gr[1] = gr[0] */
#define IMOD ((Word)0xA4) /* gr[0] % gr[1] = gr[0] */
#define IDEC ((Word)0xA5) /* gr[0] = gr[0] - 1 */
#define IINC ((Word)0xA6) /* gr[0] = gr[0] + 1 */

#define ICMPEQ ((Word)0xB0) /* gr[0] = gr[0] == gr[1] ? 1 : 0 */
#define ICMPLT ((Word)0xB1) /* gr[0] = gr[0] < gr[1] ? 1 : 0 */

class CPU {
    public:
        CPU(Word gr_count, Word mem_size);
        ~CPU();

        Word GRCount() const { return gr_count_; }
        Word MemSize() const { return mem_size_; }

        Word ReadPC() const { return pc_; }
        Word ReadSP() const { return sp_; }
        Word ReadSR() const { return sr_; }
        Word ReadGR(Word num) const { return grs_[num]; }

        void WritePC(Word value) { pc_ = value; }
        void WriteSP(Word value) { sp_ = value; }
        void ResetSR() { sr_ = 0; }

        Word ReadMem(Word address) {
            if (CheckBadAccess(address)) {
                return 0;
            } else {
                return mem_[address];
            }
        }

        void WriteMem(Word address, Word value) {
            if (!CheckBadAccess(address)) {
                mem_[address] = value;
            }
        }

        void MassWriteMem(Word address, const Word *data, Word size) {
            for (int i = 0; i < size; i++) {
                WriteMem(i+address, data[i]);
            }
        }

        void Run();
        void Halt();

    private:
        bool CheckBadAccess(Word address);

        void Load();
        void LoadR();
        void LoadM();

        void Store();
        void StoreR();

        void BranchR();
        void BranchX();
        void BranchIf();

        void Add();
        void Subtract();
        void Multiply();
        void Divide();
        void Mod();
        void Branch();
        void Decrement();
        void Increment();

        void CmpEq();
        void CmpLt();

        Word pc_;
        Word sp_;
        std::atomic<Word> sr_;
        Word *grs_;
        Word gr_count_;
        Word *mem_;
        Word mem_size_;
};

void PrintState(CPU& cpu);
void PrintMem(CPU& cpu);

#endif // ANVM_CPU_H_
