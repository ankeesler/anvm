#include "program.h"

#include <stdarg.h>

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
