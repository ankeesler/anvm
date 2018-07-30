#ifndef ANVM_PROGRAM_H_
#define ANVM_PROGRAM_H_

#include <vector>

#include "cpu.h"

class Program {
    public:
        Program(Word w, ...);

        const std::vector<Word>& Words() const { return is_; };
        void AddWord(Word w) { is_.push_back(w); }

    private:
        std::vector<Word> is_; 
};

#endif // ANVM_PROGRAM_H_
