#ifndef COSMIC_LOG_MACROS_HPP
#define COSMIC_LOG_MACROS_HPP

/**
 * @file log.hpp
 * @brief Defines lightweight runtime logging and error-reporting macros.
 */

#include "logger.hpp"

#include <sstream>

/**
 * @brief Internal helper that streams @p msg into a temporary @c std::ostringstream and forwards it to @p loggerCall.
 *
 * Used as the implementation backbone of every @c RUNTIME_* macro so that callers
 * can pass arbitrary @c << expressions as the message.
 */
#define COSMIC_RUNTIME_STREAM_LOG(loggerCall, msg) \
	do \
	{ \
		std::ostringstream cosmicRuntimeLogStream; \
		cosmicRuntimeLogStream << msg; \
		loggerCall(cosmicRuntimeLogStream.str()); \
	} while (false)

#ifdef NDEBUG
	/** @brief Emits an informational log line in debug builds; no-op in release. */
	#define RUNTIME_INFO(msg) do { } while (false)
	/** @brief Logs the components of a @c glm::vec2 value; no-op in release. */
	#define RUNTIME_INFO_VEC2_VAL(vec) do { } while (false)
	/** @brief Logs the components of a @c glm::vec2 pointed-to value; no-op in release. */
	#define RUNTIME_INFO_VEC2_PTR(vec) do { } while (false)
	/** @brief Logs the components of a @c glm::vec3 value; no-op in release. */
	#define RUNTIME_INFO_VEC3_VAL(vec) do { } while (false)
	/** @brief Logs the components of a @c glm::vec3 pointed-to value; no-op in release. */
	#define RUNTIME_INFO_VEC3_PTR(vec) do { } while (false)
	/** @brief Emits a warning log line in debug builds; no-op in release. */
	#define RUNTIME_WARNING(msg) do { } while (false)
#else
	/** @brief Emits an informational log line in debug builds. */
	#define RUNTIME_INFO(msg) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, msg)
	/** @brief Logs the components of a @c glm::vec2 value. */
	#define RUNTIME_INFO_VEC2_VAL(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec2] " << vec.x << ", " << vec.y)
	/** @brief Logs the components of a @c glm::vec2 pointed-to value. */
	#define RUNTIME_INFO_VEC2_PTR(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec2] " << vec->x << ", " << vec->y)
	/** @brief Logs the components of a @c glm::vec3 value. */
	#define RUNTIME_INFO_VEC3_VAL(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec3] " << vec.x << ", " << vec.y << ", " << vec.z)
	/** @brief Logs the components of a @c glm::vec3 pointed-to value. */
	#define RUNTIME_INFO_VEC3_PTR(vec) COSMIC_RUNTIME_STREAM_LOG(CosmicEngine::Logger::info, "[Vec3] " << vec->x << ", " << vec->y << ", " << vec->z)
	/** @brief Emits a warning log line in debug builds. */
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