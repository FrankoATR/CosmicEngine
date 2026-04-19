#ifndef COSMIC_LOG_MACROS_HPP
#define COSMIC_LOG_MACROS_HPP

/**
 * @file log.hpp
 * @brief Defines lightweight runtime logging and error-reporting macros.
 */

#include <iostream>
#include <stdexcept>

/** @brief Emits an informational runtime message to the standard output stream. */
#define RUNTIME_INFO(msg)    std::cout << "[Info] "    << msg << std::endl
/** @brief Emits a formatted 2D vector value to the standard output stream. */
#define RUNTIME_INFO_VEC2_VAL(vec)    std::cout << "[Info - Vec2] "    << vec.x << ", " << vec.y  << std::endl
/** @brief Emits a formatted 2D vector pointer value to the standard output stream. */
#define RUNTIME_INFO_VEC2_PTR(vec)    std::cout << "[Info - Vec2] "    << vec->x << ", " << vec->y  << std::endl
/** @brief Emits a formatted 3D vector value to the standard output stream. */
#define RUNTIME_INFO_VEC3_VAL(vec)    std::cout << "[Info - Vec3] "    << vec.x << ", " << vec.y  << ", " << vec.z << std::endl
/** @brief Emits a formatted 3D vector pointer value to the standard output stream. */
#define RUNTIME_INFO_VEC3_PTR(vec)    std::cout << "[Info - Vec3] "    << vec->x << ", " << vec->y  << ", " << vec->z << std::endl
/** @brief Emits a warning runtime message to the standard error stream. */
#define RUNTIME_WARNING(msg) std::cerr << "[Warning] " << msg << std::endl
/** @brief Throws a runtime error with the provided message. */
#define RUNTIME_ERROR(msg) throw std::runtime_error(std::string("[Error] ") + msg)

#endif // COSMIC_LOG_MACROS_HPP