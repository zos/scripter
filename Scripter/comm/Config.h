#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

#include <cstddef>

namespace Scripter {
namespace Config {

extern const std::string configPath;

const uint16_t nodePort = 20000;
const uint16_t overlordPort = 200001;

extern const std::vector<std::string> nodes;

}
}

#endif // CONFIG_H
