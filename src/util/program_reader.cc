#include "program_reader.h"

#include <assert.h>
#include <istream>
#include <sstream>

#include "log.h"
#include "src/program.h"

#define LOG(...) log_->Printf("program_writer", __FILE__, __LINE__, __VA_ARGS__)

const Error ProgramReader::Read(std::istream& is, Program *program) {
    LOG("Reading program from stream");

    do {
        unsigned char bytes[4];
        int readCount = is.readsome((char *)bytes, sizeof(bytes));
        if (readCount == 0) {
            break;
        } else if (readCount != sizeof(bytes)) {
            Error e;
            e << "ERROR: program underflow at word " << program->Words().size();
            e << "; got " << readCount << " bytes but wanted " << sizeof(bytes);
            LOG(e.S().c_str());
            return e;
        }
        Word w = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
        program->AddWord(w);
    } while (is);

    LOG("Done reading program from stream");
    return Error::NONE;
}
