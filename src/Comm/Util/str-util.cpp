#include "Comm/Util/str-util.h"

bool comm::util::start_with(const std::string &str, const std::string &prefix) {
    return str.rfind(prefix, 0) == 0;
}
