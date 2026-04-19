#ifndef COSMIC_LOG_MACROS_HPP
#define COSMIC_LOG_MACROS_HPP

/**
 * @file log.hpp
 * @brief Defines lightweight runtime logging and error-reporting macros.
 */

#include "logger.hpp"

#include <sstream>

#define COSMIC_RUNTIME_STREAM_LOG(loggerCall, msg) \
	do \
	{ \
		std::ostringstream cosmicRuntimeLogStream; \
		cosmicRuntimeLogStream << msg; \
		loggerCall(cosmicRuntimeLogStream.str()); \
	} while (false)

#ifdef NDEBUG
	#define RUNTIME_INFO(msg) do { } while (false)
	#define RUNTIME_INFO_VEC2_VAL(vec) do { } while (false)
	#define RUNTIME_INFO_VEC2_PTR(vec) do { } while (false)
	#define RUNTIME_INFO_VEC3_VAL(vec) do { } while (false)
	#define RUNTIME_INFO_VEC3_PTR(vec) do { } while (false)
	#define RUNTIME_WARNING(msg) do { } while (false)
#else
	#define RUNTIME_INFO(msg) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, msg)
	#define RUNTIME_INFO_VEC2_VAL(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec2] " << vec.x << ", " << vec.y)
	#define RUNTIME_INFO_VEC2_PTR(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec2] " << vec->x << ", " << vec->y)
	#define RUNTIME_INFO_VEC3_VAL(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec3] " << vec.x << ", " << vec.y << ", " << vec.z)
	#define RUNTIME_INFO_VEC3_PTR(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec3] " << vec->x << ", " << vec->y << ", " << vec->z)
	#define RUNTIME_WARNING(msg) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::warning, msg)
#endif

/** @brief Throws a runtime error with the provided message. */
#define RUNTIME_ERROR(msg) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::error, msg)
/** @brief Emits a standardized lifecycle log entry in debug builds. */
#define RUNTIME_LIFECYCLE(component, action) \
	do \
	{ \
		std::ostringstream cosmicRuntimeLifecycleStream; \
		cosmicRuntimeLifecycleStream << component; \
		CosmicEngine::Logger::lifecycle(cosmicRuntimeLifecycleStream.str(), action); \
	} while (false)

#endif // COSMIC_LOG_MACROS_HPP