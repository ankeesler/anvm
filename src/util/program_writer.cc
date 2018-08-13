#include "program_writer.h"

#include <ostream>

#include "log.h"
#include "src/program.h"

#define LOG(...) log_->Printf("program_writer", __FILE__, __LINE__, __VA_ARGS__)

static void WriteWord(Word word, std::ostream& os);

Error ProgramWriter::Write(const Program& program, std::ostream& os) {
    LOG("Writing program to stream");
    WriteWord(program.EntryAddress(), os);
    for (Word word : program.Words()) {
        WriteWord(word, os);
    }
    LOG("Done writing program to stream");
    return Error::NONE;
}

static void WriteWord(Word word, std::ostream& os) {
    os.put((word & 0xFF000000) >> 24);
    os.put((word & 0x00FF0000) >> 16);
    os.put((word & 0x0000FF00) >> 8);
    os.put((word & 0x000000FF) >> 0);
}
