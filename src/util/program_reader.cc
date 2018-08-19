#include "program_reader.h"

#include <assert.h>
#include <istream>
#include <sstream>

#include "log.h"
#include "src/program.h"

#define LOG(...) log_->Printf("program_writer", __FILE__, __LINE__, __VA_ARGS__)

static Word MakeWord(unsigned char *bytes);
static void MakeUnderflowError(Error *error, int index, int read_count);

const Error ProgramReader::Read(std::istream& is, Program *program) {
    LOG("Reading program from stream");

    unsigned char bytes[4];
    if (!is.read((char *)bytes, sizeof(bytes))) {
        Error e;
        e << "Could not read program entry address: ";
        MakeUnderflowError(&e, 0, is.gcount());
        LOG(e.S().c_str());
        return e;
    }
    Word entry_addr = MakeWord(bytes);
    program->SetEntryAddress(entry_addr);
    LOG("Read entry address: 0x%08X", entry_addr);

    while (is.read((char *)bytes, sizeof(bytes))) {
        Word w = MakeWord(bytes);
        program->AddWord(w);
        LOG("Read program word (%d): 0x%08X", program->Words().size(), w);
    }
    if (is.gcount() != 0) {
        Error e;
        e << "Could not read program text: ";
        MakeUnderflowError(&e, program->Words().size()+1, is.gcount());
        LOG(e.S().c_str());
        return e;
    }

    LOG("Done reading program from stream");
    return Error::NONE;
}

Word MakeWord(unsigned char *bytes) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
}

static void MakeUnderflowError(Error *error, int index, int read_count) {
    (*error) << "Failed to read byte " << read_count << " of word " << index;
}
