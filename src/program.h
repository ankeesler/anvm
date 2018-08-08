#ifndef ANVM_PROGRAM_H_
#define ANVM_PROGRAM_H_

#include <vector>

#include "cpu.h"

class Program {
    public:
        Program() { }

        Program(Word w, ...);

        const std::vector<Word>& Words() const { return is_; };
        void AddWord(Word w) { is_.push_back(w); }
        void AddWord(Word address, Word w);
        void AddWords(Word address, const std::vector<Word> words);

    private:
        std::vector<Word> is_; 
};

#endif // ANVM_PROGRAM_H_
