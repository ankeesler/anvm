#include "program.h"

#include <stdarg.h>
#include <vector>

#include "cpu.h"

Program::Program(Word w, ...) : entry_address_(0) {
    va_list list;
    va_start(list, w);

    while (w != IEXIT) {
        words_.push_back(w);
        w = va_arg(list, Word);
    }
    words_.push_back(IEXIT);

    va_end(list);
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
