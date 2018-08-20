#ifndef ANVM_UTIL_PROGRAM_LIBRARY_H
#define ANVM_UTIL_PROGRAM_LIBRARY_H

#include "src/program.h"
#include "src/cpu.h"

class ProgramLibrary {
    public:
        ProgramLibrary(const ProgramLibrary& pl) = delete;
        ProgramLibrary(const ProgramLibrary&& pl) = delete;

        static const ProgramLibrary& Instance() {
            static ProgramLibrary instance;
            if (!instance.IsInitialize()) {
                instance.Initialize();
            }
            return instance;
        }

        const Program& InfiniteLoop() const { return infinite_loop_; }

    private:
        ProgramLibrary() : initialized_(false) { }

        bool IsInitialize() const { return initialized_; }

        void Initialize() {
            const std::vector<Word> words { CPU::IBRANCHX, 0, CPU::IEXIT, };
            infinite_loop_.AddWords(0, words);
        }

        bool initialized_;
        Program infinite_loop_;
};

#endif // ANVM_UTIL_PROGRAM_LIBRARY_H
