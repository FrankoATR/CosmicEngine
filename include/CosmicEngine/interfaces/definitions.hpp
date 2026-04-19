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

#define GAMEMANAGE_HEADER <CosmicEngine/managers/game_manager.hpp>
#define GM_MN CosmicEngine::GameManager::GetInstance()

#define INPUTMANAGER_HEADER <CosmicEngine/managers/input/input_manager.hpp>
#define INP_MN CosmicEngine::InputManager::GetInstance()

#define CAMERAMANAGER_HEADER <CosmicEngine/managers/camera/camera_manager.hpp>
#define CAM_MN CosmicEngine::CameraManager::GetInstance()

#define RESOURCEMANAGER_HEADER <CosmicEngine/managers/resource/resource_manager.hpp>
#define RS_MN CosmicEngine::ResourceManager::GetInstance()

#define OBJECTMANAGER_HEADER <CosmicEngine/managers/object/object_manager.hpp>
#define OBJ_MN CosmicEngine::ObjectManager::GetInstance()

#define BODYMANAGER_HEADER <CosmicEngine/managers/body/body_manager.hpp>
#define BOD_MN CosmicEngine::BodyManager::GetInstance()

#define AUDIOMANAGER_HEADER <CosmicEngine/managers/audio/audio_manager.hpp>
#define AUD_MN CosmicEngine::AudioManager::GetInstance()

#define EVENTMANAGER_HEADER <CosmicEngine/managers/event/event_manager.hpp>
#define EVT_MN CosmicEngine::EventManager::GetInstance()

#define TIMERMANAGER_HEADER <CosmicEngine/managers/timer/timer_manager.hpp>
#define TMR_MN CosmicEngine::TimerManager::GetInstance()

#define SCHEDULEMANAGER_HEADER <CosmicEngine/managers/schedule/schedule_manager.hpp>
#define SCH_MN CosmicEngine::ScheduleManager::GetInstance()

#define ANIMATIONMANAGER_HEADER <CosmicEngine/managers/animation/animation_manager.hpp>
#define ANM_MN CosmicEngine::AnimationManager::GetInstance()

#define SCENEMANAGER_HEADER <CosmicEngine/managers/scene/scene_manager.hpp>
#define SCN_MN CosmicEngine::SceneManager::GetInstance()

#define UIMANAGER_HEADER <CosmicEngine/managers/ui/ui_manager.hpp>
#define UI_MN CosmicEngine::UIManager::GetInstance()

#define SQLMANAGER_HEADER <CosmicEngine/managers/storage/sql/sql_manager.hpp>
#define SQL_MN CosmicEngine::SQLManager::GetInstance()

#define JSONMANAGER_HEADER <CosmicEngine/managers/storage/json/json_manager.hpp>
#define JSON_MN CosmicEngine::JsonManager::GetInstance()

#define NETWORKMANAGER_HEADER <CosmicEngine/managers/network/network_manager.hpp>
#define NET_MN CosmicEngine::NetworkManager::GetInstance()

#define GAMEGRIDCOLLISIONS_HEADER <CosmicEngine/collisions/GameGridCollisions.hpp>



#define OBJECT_HEADER <CosmicEngine/models/object/object.hpp>
#define BODY_HEADER <CosmicEngine/models/body/body.hpp>



#endif // COSMIC_DEFINITIONS_HPP