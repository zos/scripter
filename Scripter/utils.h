#ifndef UTILS_H
#define UTILS_H
#include <sstream>


inline unsigned strToUnsigned(const std::string &s) {
    unsigned value;
    std::stringstream ss(s);
    ss >> value;
    return value;
}


#endif // UTILS_H
