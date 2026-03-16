#ifndef COSMIC_LOG_MACROS_HPP
#define COSMIC_LOG_MACROS_HPP

#include <iostream>
#include <stdexcept>

#define RUNTIME_INFO(msg)    std::cout << "[Info] "    << msg << std::endl
#define RUNTIME_INFO_VEC2_VAL(vec)    std::cout << "[Info - Vec2] "    << vec.x << ", " << vec.y  << std::endl
#define RUNTIME_INFO_VEC2_PTR(vec)    std::cout << "[Info - Vec2] "    << vec->x << ", " << vec->y  << std::endl
#define RUNTIME_INFO_VEC3_VAL(vec)    std::cout << "[Info - Vec3] "    << vec.x << ", " << vec.y  << ", " << vec.z << std::endl
#define RUNTIME_INFO_VEC3_PTR(vec)    std::cout << "[Info - Vec3] "    << vec->x << ", " << vec->y  << ", " << vec->z << std::endl
#define RUNTIME_WARNING(msg) std::cerr << "[Warning] " << msg << std::endl
#define RUNTIME_ERROR(msg) throw std::runtime_error(std::string("[Error] ") + msg)

#endif // COSMIC_LOG_MACROS_HPP