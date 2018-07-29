#include "system.h"

#include "program.h"
#include "cpu.h"

Word System::Run(const Program& p) {
    const std::vector<Word>& is = p.Instructions();

    // Start from a fresh CPU.
    cpu_->WritePC(0);
    cpu_->WriteSP(0);
    cpu_->ResetSR();

    // Load program into memory.
    cpu_->MassWriteMem(0, &is.front(), is.size());
    Word status = cpu_->ReadSR();
    if (status & STATUS_FAILURE) {
        return status;
    }

    // Run program.
    cpu_->Run();

    // Check status.
    return cpu_->ReadSR();
}
