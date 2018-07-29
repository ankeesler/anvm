#include "program.h"

#include <stdarg.h>

#include "cpu.h"

Program::Program(Word i, ...) {
    va_list list;
    va_start(list, i);

    while (i != IEXIT) {
        is_.push_back(i);
        i = va_arg(list, Word);
    }
    is_.push_back(IEXIT);

    va_end(list);
}
