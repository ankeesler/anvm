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
    int read_count = is.readsome((char *)bytes, sizeof(bytes));
    if (read_count != sizeof(bytes)) {
        Error e;
        MakeUnderflowError(&e, 0, read_count);
        LOG(e.S().c_str());
        return e;
    }
    Word entry_addr = MakeWord(bytes);
    program->SetEntryAddress(entry_addr);

    do {
        read_count = is.readsome((char *)bytes, sizeof(bytes));
        if (read_count == 0) {
            break;
        } else if (read_count != sizeof(bytes)) {
            Error e;
            MakeUnderflowError(&e, program->Words().size()+1, read_count);
            LOG(e.S().c_str());
            return e;
        }
        Word w = MakeWord(bytes);
        program->AddWord(w);
    } while (is);

    LOG("Done reading program from stream");
    return Error::NONE;
}

Word MakeWord(unsigned char *bytes) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
}

static void MakeUnderflowError(Error *error, int index, int read_count) {
    (*error) << "ERROR: program underflow at word " << index;
    (*error) << "; got " << read_count << " bytes but wanted 4";
}
