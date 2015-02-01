#pragma once

#include <iostream>
#include <string>

#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define RESET_COLOR "\033[0m"

#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_PRINT 2
#define LOG_LEVEL_ERROR 1

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_PRINT
#endif

#define LOG(msg) do { \
    std::string _file = __FILE__; \
    auto pos = _file.find_last_of('/'); \
    _file = _file.substr(pos + 1); \
    std::cout << _file << ":" << __LINE__ << ": " << msg << std::endl; \
    } while(0);

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOGD(msg) std::cout << msg << std::endl;
#else
#define LOGD(msg)
#endif

#if LOG_LEVEL >= LOG_LEVEL_PRINT
#define LOGP(msg) std::cout << YELLOW_COLOR << msg << RESET_COLOR << std::endl;
#else
#define LOGP(msg)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOGI(msg) std::cout << GREEN_COLOR << msg << RESET_COLOR << std::endl;
#else
#define LOGI(msg)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOGE(msg) std::cout << RED_COLOR << msg << RESET_COLOR << std::endl;
#else
#define LOGE(msg)
#endif
