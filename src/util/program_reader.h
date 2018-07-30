#ifndef ANVM_UTIL_PROGRAM_READER_H_
#define ANVM_UTIL_PROGRAM_READER_H_

#include <istream>

#include "log.h"
#include "src/program.h"

class ProgramReader {
    public:
        class Error {
            public:
                Error() : exists_(false) { }

                operator bool() const { return exists_; }

                const std::string& Get() const { return s_; }
                void Set(const std::string& s) { s_ = s; exists_ = true; }

            private:
                bool exists_;
                std::string s_;
        };

        ProgramReader(Log *log) : log_(log) { }

        const Error& Read(std::istream& is, Program *program);

    private:
        Log *log_;
        Error error_;
};

#endif // ANVM_UTIL_PROGRAM_READER_H_
