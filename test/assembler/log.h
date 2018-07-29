#include <stdarg.h>

#include "src/assembler/log.h"

class TestLog : public Log {
    public:
        TestLog() : debug_(false) { }
        TestLog(bool debug) : debug_(debug) { }

        void Printf(
                const char *who,
                const char *file,
                int line,
                const char *format,
                ...) {
            if (debug_) {
                va_list list;
                va_start(list, format);
                printf("[anasm] [%s] [%s:%d] ", who, file, line);
                vprintf(format, list);
                printf("\n");
                va_end(list);
            }
        }

    private:
        bool debug_;
};

