#ifndef ANVM_ASSEMBLER_ASSEMBLER_H_
#define ANVM_ASSEMBLER_ASSEMBLER_H_

#include <istream>
#include <ostream>

#include "src/util/error.h"
#include "src/util/log.h"

class Assembler {
    public:
        Error Run(Log *log, std::istream& is, std::ostream& os);
};

#endif // ANVM_ASSEMBLER_ASSEMBLER_H_
