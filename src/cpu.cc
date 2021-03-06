#include "cpu.h"

#include <stdio.h>
#include <iostream>
#include <functional>

CPU::CPU(Word gr_count, Word mem_size) {
    gr_count_ = gr_count;
    grs_ = new Word[gr_count];
    bzero(grs_, gr_count * sizeof(Word));

    mem_size_ = mem_size;
    mem_ = new Word[mem_size];
    bzero(mem_, mem_size * sizeof(Word));
}

CPU::~CPU() {
    delete grs_;
    delete mem_;
}

void CPU::Run() {
    while (true) {
        //PrintState(*this);
        //PrintMem(*this);
        switch (ReadMem(pc_)) {
            case IEXIT: return;

            case CPU::CPU::ILOAD: Load(); break;
            case CPU::CPU::ILOADR: LoadR(); break;
            case CPU::CPU::ILOADM: LoadM(); break;

            case CPU::ISTORE: Store(); break;
            case CPU::ISTORER: StoreR(); break;

            case CPU::IBRANCH: Branch(); break;
            case CPU::IBRANCHR: BranchR(); break;
            case CPU::IBRANCHX: BranchX(); break;
            case CPU::IBRANCHIF: BranchIf(); break;

            case CPU::IADD: Add(); break;
            case CPU::ISUBTRACT: Subtract(); break;
            case CPU::IMULTIPLY: Multiply(); break;
            case CPU::IDIVIDE: Divide(); break;
            case CPU::IMOD: Mod(); break;
            case CPU::IDEC: Decrement(); break;
            case CPU::IINC: Increment(); break;

            case CPU::ICMPEQ: CmpEq(); break;
            case CPU::ICMPLT: CmpLt(); break;

            default:
                sr_ |= UNKNOWNI;
                return;
        }

        if (sr_ &= FAILURE) {
            return;
        }

        pc_++;
    }
}

void CPU::Halt() {
    sr_ |= HALTED;
}

bool CPU::CheckBadAccess(Word address) {
    if (address >= MemSize()) {
        sr_ |= BADACCESS;
        return true;
    }
    return false;
}

void CPU::Load() {
    Word value = ReadMem(++pc_);
    Word reg = ReadMem(++pc_);
    if (reg >= GRCount()) {
        sr_ |= BADREGISTER;
        return;
    }
    if (reg == SP) {
        WriteSP(value);
    } else {
        grs_[reg] = value;
    }
}

void CPU::LoadR() {
    Word pcStart = pc_;
    Word regA = ReadMem(++pc_);
    Word regB = ReadMem(++pc_);
    if (regA >= GRCount() || regB >= GRCount()) {
        sr_ |= BADREGISTER;
        return;
    }
    if (regA == SP && regB == SP) {
        // no op
    } else if (regA == PC && regB == PC) {
        // no op
    } else if (regA == SP) {
        grs_[regB] = ReadSP();
    } else if (regA == PC) {
        grs_[regB] = pcStart;
    } else if (regB == SP) {
        WriteSP(grs_[regA]);
    } else if (regB == PC) {
        WritePC(grs_[regB]);
    } else {
        grs_[regB] = grs_[regA];
    }
}

void CPU::LoadM() {
    Word addr = ReadMem(++pc_);
    Word reg = ReadMem(++pc_);
    if (reg >= GRCount()) {
        sr_ |= BADREGISTER;
        return;
    }
    grs_[reg] = ReadMem(addr);
}

void CPU::Store() {
    Word literal = ReadMem(++pc_);
    Word addr = ReadMem(++pc_);
    WriteMem(addr, literal);
}

void CPU::StoreR() {
    Word reg = ReadMem(++pc_);
    Word addr = ReadMem(++pc_);
    if (reg >= GRCount()) {
        sr_ |= BADREGISTER;
        return;
    }
    WriteMem(addr, grs_[reg]);
}

void CPU::Branch() {
    pc_ = grs_[0] - 1; // account for automatic pc bump
}

void CPU::BranchR() {
    Word reg = ReadMem(++pc_);
    if (reg >= GRCount()) {
        sr_ |= BADREGISTER;
        return;
    }
    pc_ = grs_[reg] - 1; // account for auto pc bump
}

void CPU::BranchX() {
    Word addr = ReadMem(++pc_);
    pc_ = addr - 1; // account for automatic pc bump
}

void CPU::BranchIf() {
    if (grs_[0] != 0) {
        pc_ = grs_[1] - 1; // account for automatic pc bump
    }
}

void CPU::Add() {
    grs_[0] = grs_[0] + grs_[1];
}

void CPU::Subtract() {
    grs_[0] = grs_[0] - grs_[1];
}

void CPU::Multiply() {
    grs_[0] = grs_[0] * grs_[1];
}

void CPU::Divide() {
    if (grs_[1] == 0) {
        sr_ |= DIVZERO;
        return;
    }
    grs_[0] = grs_[0] / grs_[1];
}

void CPU::Mod() {
    grs_[0] = grs_[0] % grs_[1];
}

void CPU::Decrement() {
    grs_[0] = grs_[0] - 1;
}

void CPU::Increment() {
    grs_[0] = grs_[0] + 1;
}

void CPU::CmpEq() {
    grs_[0] = grs_[0] == grs_[1] ? 1 : 0;
}

void CPU::CmpLt() {
    grs_[0] = grs_[0] < grs_[1] ? 1 : 0;
}

void PrintState(CPU& cpu) {
    Word pc = cpu.ReadPC();
    Word sp = cpu.ReadSP();
    Word sr = cpu.ReadSR();
    printf("---STATE---\n");
    printf("PC  = 0x%08X (0x%08X, 0x%08X, 0x%08X, ...)\n", pc,
            cpu.ReadMem(pc+0), cpu.ReadMem(pc+1), cpu.ReadMem(pc+2));
    printf("SP  = 0x%08X\n", sp);
    printf("SR  = 0x%08X\n", sr);
    for (int i = 0; i < cpu.GRCount(); i++) {
        printf("GR%d = 0x%08X\n", i, cpu.ReadGR(i));
    }
}

void PrintMem(CPU& cpu) {
    printf("---MEM---\n");
    printf("    0          1          2          3\n");
    printf("%2d: ", 0);
    for (int i = 0; i < cpu.MemSize(); i++) {
        if (i > 0 && i+1 != cpu.MemSize() && i % 4 == 0) {
            printf("\n");
            printf("%2d: ", i);
        }
        printf("0x%08X ", cpu.ReadMem(i));
    }
    printf("\n");
}
