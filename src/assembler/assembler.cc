#include "assembler.h"

#include <assert.h>
#include <istream>
#include <ostream>

#include "parser.h"
#include "linker.h"
#include "src/util/program_writer.h"
#include "src/util/error.h"
#include "src/util/log.h"

Error Assembler::Run(Log *log, std::istream& is, std::ostream& os) {
    Parser parser(log);
    Parser::Result parserResult;
    parser.Parse(is, &parserResult);
    if (parserResult.Errors().size() > 0) {
        assert(parserResult.Errors().size() == 1);
        return parserResult.Errors()[0];
    }

    Linker linker(log);
    Program program;
    const char *error = linker.Link(parserResult, &program);
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
