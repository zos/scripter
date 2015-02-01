#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

typedef char byte_t;
typedef std::vector<byte_t> buffer;

std::size_t object_size(const char* s);

template<typename T>
std::size_t object_size(T const& obj) {
    return sizeof(obj);
}

template<typename T>
buffer serialize(const T& obj) {
    std::size_t size = object_size(obj);
    buffer buf(size);

    byte_t const* obj_begin = reinterpret_cast<byte_t const*>(&obj);
    std::copy(obj_begin, obj_begin + size, buf.begin());

    return buf;
}

template<>
buffer serialize<std::string>(std::string const& str);

template<typename T>
T deserialize(buffer const& buf) {
    return *reinterpret_cast<const T*>(&buf[0]);
}

template<>
inline std::string deserialize<std::string>(buffer const& buf) {
    return deserialize<char*>(buf);
}

#endif // SERIALIZATION_H
