#include "program.h"

#include <stdarg.h>
#include <vector>

#include "cpu.h"

Program::Program(Word w, ...) {
    va_list list;
    va_start(list, w);

    while (w != IEXIT) {
        is_.push_back(w);
        w = va_arg(list, Word);
    }
    is_.push_back(IEXIT);

    va_end(list);
}

void Program::AddWord(Word address, Word w) {
    int is_size = is_.size();
    if (is_size < address + 1) {
        for (int i = 0; i < address - is_size + 1; i++) {
            is_.push_back(0);
        }
    }

    is_[address] = w;
}

void Program::AddWords(Word address, std::vector<Word> words) {
    int is_size = is_.size();
    int words_size = words.size();
    if (is_size < address + words_size) {
        for (int i = 0; i < address - is_size + words_size; i++) {
            is_.push_back(0);
        }
    }

    for (int i = address; i < address + words_size; i++) {
        is_[i] = words[i-address];
    }
}
