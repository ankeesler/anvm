#ifndef ANVM_PROGRAM_H_
#define ANVM_PROGRAM_H_

#include <vector>

#include "cpu.h"

class Program {
    public:
        Program(Word i, ...);

        const std::vector<Word>& Instructions() const { return is_; };

    private:
        std::vector<Word> is_; 
};

#endif // ANVM_PROGRAM_H_
