#include "assembler.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <vector>

#include "parser.h"
#include "linker.h"
#include "symbol_table_populator.h"
#include "symbol_table.h"
#include "src/util/program_writer.h"
#include "src/util/error.h"
#include "src/util/log.h"

Error Assembler::Run(Log *log, std::istream& is, std::ostream& os) {
    Parser parser(log);
    SymbolTable st(log);
    SymbolTablePopulator stp(log, &st);
    parser.Parse(is, &stp);
    const std::vector<Error>& errors = stp.Errors();
    if (errors.size() > 0) {
        std::stringstream ss("ERROR(s): ");
        ss << "ERROR(s) during parsing:";
        for (const Error& error : errors) {
            ss << std::endl << "- " << error.S();
        }
        return Error(ss.str());
    }

    Linker linker(log);
    Program program;
    Error err1 = linker.Link(st, &program);
    if (err1) {
        return err1;
    }

    ProgramWriter writer(log);
    Error err2 = writer.Write(program, os);
    if (err2) {
        return err2;
    }

    return Error::NONE;
}
