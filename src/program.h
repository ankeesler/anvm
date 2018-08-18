#ifndef ANVM_PROGRAM_H_
#define ANVM_PROGRAM_H_

#include <vector>

#include "cpu.h"

class Program {
    public:
        Program() : entry_address_(0) { }

        Program(Word w, ...);

        bool operator==(const Program& p) const;
        bool operator!=(const Program& p) const;

        Word EntryAddress() const { return entry_address_; }
        void SetEntryAddress(Word address) { entry_address_ = address; }

        const std::vector<Word>& Words() const { return words_; };
        void AddWord(Word w) { words_.push_back(w); }
        void AddWord(Word address, Word w);
        void AddWords(Word address, const std::vector<Word> words);

    private:
        Word entry_address_;
        std::vector<Word> words_;
};

#endif // ANVM_PROGRAM_H_
