#include "assembler.h"

#include <istream>
#include <ostream>

#include "parser.h"
#include "linker.h"
#include "src/util/program_writer.h"
#include "src/util/error.h"
#include "src/util/log.h"

Error Assembler::Run(Log *log, std::istream& is, std::ostream& os) {
    Parser parser(log);
    const Parser::Result& result = parser.Parse(is);
    std::string maybeError;
    if (result.Error(&maybeError)) {
        return Error(maybeError);
    }

    Linker linker(log);
    Program program;
    const char *error = linker.Link(result, &program);
    if (error != nullptr) {
        return Error(error);
    }

    ProgramWriter writer(log);
    Error err = writer.Write(program, os);
    if (err) {
        return err;
    }

    return Error::NONE;
}
