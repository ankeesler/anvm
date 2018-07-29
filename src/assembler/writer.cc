#include "writer.h"

#include <ostream>

#include "parser.h"

#define LOG(...) log_->Printf("writer", __FILE__, __LINE__, __VA_ARGS__)

const char *Writer::Write(const Parser::Result& result, std::ostream& os) {
    LOG("Starting writer");
    return nullptr;
}
