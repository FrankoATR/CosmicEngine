#ifndef WAND_LOG_MACROS_HPP
#define WAND_LOG_MACROS_HPP

#include <iostream>
#include <stdexcept>

#define RUNTIME_INFO(msg)    std::cout << "[Info] "    << msg << std::endl
#define RUNTIME_WARNING(msg) std::cerr << "[Warning] " << msg << std::endl
#define RUNTIME_ERROR(msg) throw std::runtime_error(std::string("[Error] ") + msg)

#endif // WAND_LOG_MACROS_HPP
