#ifndef ANVM_CPU_H_
#define ANVM_CPU_H_

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <atomic>

typedef int32_t Word;
#define MAX_WORD INT32_MAX

class CPU {
    public:
        enum {
            UNKNOWNI    = ((Word)1),
            DIVZERO     = ((Word)2),
            BADACCESS   = ((Word)4),
            BADREGISTER = ((Word)8),
            HALTED      = ((Word)16),

            FAILURE = (UNKNOWNI | DIVZERO | BADACCESS | BADREGISTER | HALTED),
        } Status;

        enum {
            SP = ((Word)-2),
            PC = ((Word)-3),
        } Register;

        enum {
            IEXIT = ((Word)-1), /* the program is done */

            // Load into a register.
            ILOAD  = ((Word)0x00), /* literal, reg; gr[reg] = literal */
            ILOADR = ((Word)0x01), /* regA, regB; gr[regB] = gr[regA] */
            ILOADM = ((Word)0x02), /* addr, reg; mem[addr] = gr[reg] */

            // Store into memory.
            ISTORE  = ((Word)0x10), /* literal, addr; mem[addr] = literal */
            ISTORER = ((Word)0x11), /* reg, addr; mem[addr] = gr[reg] */

            // Branch to instruction.
            IBRANCH   = ((Word)0x20), /* pc = gr[0] */
            IBRANCHR  = ((Word)0x21), /* reg; pc = gr[reg] */
            IBRANCHX  = ((Word)0x23), /* addr; pc = addr */
            IBRANCHIF = ((Word)0x24), /* if gr[0] != 0, pc = gr[1] */

            // Perform math operations.
            IADD      = ((Word)0xA0), /* gr[0] + gr[1] = gr[0] */
            IMULTIPLY = ((Word)0xA1), /* gr[0] * gr[1] = gr[0] */
            ISUBTRACT = ((Word)0xA2), /* gr[0] - gr[1] = gr[0] */
            IDIVIDE   = ((Word)0xA3), /* gr[0] / gr[1] = gr[0] */
            IMOD      = ((Word)0xA4), /* gr[0] % gr[1] = gr[0] */
            IDEC      = ((Word)0xA5), /* gr[0] = gr[0] - 1 */
            IINC      = ((Word)0xA6), /* gr[0] = gr[0] + 1 */

            ICMPEQ = ((Word)0xB0), /* gr[0] = gr[0] == gr[1] ? 1 : 0 */
            ICMPLT = ((Word)0xB1), /* gr[0] = gr[0] < gr[1] ? 1 : 0 */
        } Instruction;

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
