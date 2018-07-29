#ifndef ANVM_ASSEMBLER_LOG_H_
#define ANVM_ASSEMBLER_LOG_H_

#include <stdarg.h>

class Log {
    public:
        virtual ~Log() { }

        virtual void Printf(
                const char *who,
                const char *file,
                int line,
                const char *format,
                ...)
            = 0;
};

class VoidLog : public Log { 
    public:
        void Printf(
                const char *who,
                const char *file,
                int line,
                const char *format,
                ...) {
            // do nothing
        }
};

class StdoutLog : public Log {
    public:
        void Printf(
                const char *who,
                const char *file,
                int line,
                const char *format,
                ...) {
            va_list list;
            va_start(list, format);
            printf("[anasm] [%s] [%s:%d] ", who, file, line);
            vprintf(format, list);
            printf("\n");
            va_end(list);
        }
};

#endif // ANVM_ASSEMBLER_LOG_H_
