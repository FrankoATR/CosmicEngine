#ifndef COSMIC_DEFINITIONS_HPP
#define COSMIC_DEFINITIONS_HPP

/**
 * @file definitions.hpp
 * @brief Defines convenience include and singleton-access macros for the engine.
 *
 * Including this header gives you short-hand macros for every engine manager.
 * Use the _HEADER macros with @c \#include to bring in the right header, and
 * use the short macros (e.g. @c GM_MN, @c INP_MN) as direct singleton references.
 *
 * @par Example — using the convenience macros
 * @code
 * #include <CosmicEngine/interfaces/definitions.hpp>
 * #include GAMEMANAGE_HEADER
 * #include INPUTMANAGER_HEADER
 * #include RESOURCEMANAGER_HEADER
 * #include OBJECTMANAGER_HEADER
 * #include BODYMANAGER_HEADER
 *
 * // Then in any scene or entity:
 * GM_MN.endprogram();         // GameManager singleton
 * INP_MN.IsKeyPressed(GLFW_KEY_W, CosmicEngine::KeyRelease);
 * RS_MN.LoadTexture("key", "path.png");
 * OBJ_MN.Add(new MyEntity(...));
 * BOD_MN.Add(new CosmicEngine::Body(...));
 * @endcode
 */

/** @brief Header path for GameManager (use with @c \#include). */
#define GAMEMANAGE_HEADER <CosmicEngine/managers/game_manager.hpp>
/** @brief Singleton accessor for GameManager. */
#define GM_MN CosmicEngine::GameManager::GetInstance()

/** @brief Header path for InputManager. */
#define INPUTMANAGER_HEADER <CosmicEngine/managers/input/input_manager.hpp>
/** @brief Singleton accessor for InputManager. */
#define INP_MN CosmicEngine::InputManager::GetInstance()

/** @brief Header path for CameraManager. */
#define CAMERAMANAGER_HEADER <CosmicEngine/managers/camera/camera_manager.hpp>
/** @brief Singleton accessor for CameraManager. */
#define CAM_MN CosmicEngine::CameraManager::GetInstance()

/** @brief Header path for ResourceManager. */
#define RESOURCEMANAGER_HEADER <CosmicEngine/managers/resource/resource_manager.hpp>
/** @brief Singleton accessor for ResourceManager. */
#define RS_MN CosmicEngine::ResourceManager::GetInstance()

/** @brief Header path for ObjectManager. */
#define OBJECTMANAGER_HEADER <CosmicEngine/managers/object/object_manager.hpp>
/** @brief Singleton accessor for ObjectManager. */
#define OBJ_MN CosmicEngine::ObjectManager::GetInstance()

/** @brief Header path for BodyManager. */
#define BODYMANAGER_HEADER <CosmicEngine/managers/body/body_manager.hpp>
/** @brief Singleton accessor for BodyManager. */
#define BOD_MN CosmicEngine::BodyManager::GetInstance()

/** @brief Header path for AudioManager. */
#define AUDIOMANAGER_HEADER <CosmicEngine/managers/audio/audio_manager.hpp>
/** @brief Singleton accessor for AudioManager. */
#define AUD_MN CosmicEngine::AudioManager::GetInstance()

/** @brief Header path for EventManager. */
#define EVENTMANAGER_HEADER <CosmicEngine/managers/event/event_manager.hpp>
/** @brief Singleton accessor for EventManager. */
#define EVT_MN CosmicEngine::EventManager::GetInstance()

/** @brief Header path for TimerManager. */
#define TIMERMANAGER_HEADER <CosmicEngine/managers/timer/timer_manager.hpp>
/** @brief Singleton accessor for TimerManager. */
#define TMR_MN CosmicEngine::TimerManager::GetInstance()

/** @brief Header path for ScheduleManager. */
#define SCHEDULEMANAGER_HEADER <CosmicEngine/managers/schedule/schedule_manager.hpp>
/** @brief Singleton accessor for ScheduleManager. */
#define SCH_MN CosmicEngine::ScheduleManager::GetInstance()

/** @brief Header path for AnimationManager. */
#define ANIMATIONMANAGER_HEADER <CosmicEngine/managers/animation/animation_manager.hpp>
/** @brief Singleton accessor for AnimationManager. */
#define ANM_MN CosmicEngine::AnimationManager::GetInstance()

/** @brief Header path for LightManager. */
#define LIGHTMANAGER_HEADER <CosmicEngine/managers/light/light_manager.hpp>
/** @brief Singleton accessor for LightManager. */
#define LGT_MN CosmicEngine::LightManager::GetInstance()

/** @brief Header path for SceneManager. */
#define SCENEMANAGER_HEADER <CosmicEngine/managers/scene/scene_manager.hpp>
/** @brief Singleton accessor for SceneManager. */
#define SCN_MN CosmicEngine::SceneManager::GetInstance()

/** @brief Header path for UIManager. */
#define UIMANAGER_HEADER <CosmicEngine/managers/ui/ui_manager.hpp>
/** @brief Singleton accessor for UIManager. */
#define UI_MN CosmicEngine::UIManager::GetInstance()

/** @brief Header path for SQLManager. */
#define SQLMANAGER_HEADER <CosmicEngine/managers/storage/sql/sql_manager.hpp>
/** @brief Singleton accessor for SQLManager. */
#define SQL_MN CosmicEngine::SQLManager::GetInstance()

/** @brief Header path for JsonManager. */
#define JSONMANAGER_HEADER <CosmicEngine/managers/storage/json/json_manager.hpp>
/** @brief Singleton accessor for JsonManager. */
#define JSON_MN CosmicEngine::JsonManager::GetInstance()

/** @brief Header path for NetworkManager. */
#define NETWORKMANAGER_HEADER <CosmicEngine/managers/network/network_manager.hpp>
/** @brief Singleton accessor for NetworkManager. */
#define NET_MN CosmicEngine::NetworkManager::GetInstance()

/** @brief Header path for GameGridCollisions. */
#define GAMEGRIDCOLLISIONS_HEADER <CosmicEngine/collisions/game_grid_collisions.hpp>



/** @brief Header path for the runtime Object base class. */
#define OBJECT_HEADER <CosmicEngine/models/object/object.hpp>
/** @brief Header path for the runtime Body class. */
#define BODY_HEADER <CosmicEngine/models/body/body.hpp>



#endif // COSMIC_DEFINITIONS_HPP