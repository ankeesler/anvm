#include "program.h"

#include <stdarg.h>
#include <vector>

#include "cpu.h"

Program::Program(Word w, ...) : entry_address_(0) {
    va_list list;
    va_start(list, w);

    while (w != CPU::IEXIT) {
        words_.push_back(w);
        w = va_arg(list, Word);
    }
    words_.push_back(CPU::IEXIT);

    va_end(list);
}

bool Program::operator==(const Program& p) const {
    if (words_.size() != p.words_.size()) {
        return false;
    } else if (!std::equal(words_.begin(), words_.end(), p.words_.begin())) {
        return false;
    } else if (entry_address_ != p.entry_address_) {
        return false;
    }

    return true;
}

bool Program::operator!=(const Program& p) const {
    return !(*this == p);
}

void Program::AddWord(Word address, Word w) {
    int is_size = words_.size();
    if (is_size < address + 1) {
        for (int i = 0; i < address - is_size + 1; i++) {
            words_.push_back(0);
        }
    }

    words_[address] = w;
}

void Program::AddWords(Word address, std::vector<Word> words) {
    int is_size = words_.size();
    int words_size = words.size();
    if (is_size < address + words_size) {
        for (int i = 0; i < address - is_size + words_size; i++) {
            words_.push_back(0);
        }
    }

    for (int i = address; i < address + words_size; i++) {
        words_[i] = words[i-address];
    }
}
