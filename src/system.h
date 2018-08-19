#ifndef ANVM_SYSTEM_H_
#define ANVM_SYSTEM_H_

#include "cpu.h"
#include "program.h"

class System {
    public:
        System(CPU *cpu) : cpu_(cpu) { }

        Word Run(const Program& p);
        void Halt();

    private:
        CPU *cpu_;
};

#endif // ANVM_SYSTEM_H_
