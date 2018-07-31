#include "error.h"

#include <string>

const Error Error::NONE;

std::ostream& operator<<(std::ostream& os, const Error& e) {
    return os << e.S();
}
