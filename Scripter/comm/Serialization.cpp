#include "Serialization.h"

std::size_t object_size(const char* s) {
    return std::strlen(s);
}

template<>
buffer serialize<std::string>(std::string const& str) {
    return serialize(str.c_str());
}
