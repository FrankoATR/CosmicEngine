cmake_minimum_required(VERSION 3.10)

project(EngineTest)

set(EXEC_NAME "GeometryClone")
set(EXEC_ICON ${CMAKE_SOURCE_DIR}/assets/geometrydash.ico)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(GAME_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/Scenes/MainScene.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Player.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/SolidBlock.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Spike.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Orb.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/EndLevel.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Ground.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Background.cpp
)

set(ENGINE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/GameScene.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/GameObject.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/GameBodyObject.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/GameTimer.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UIElement.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UIElements/UIButton.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UIElements/UIText.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/ResourceManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/ObjectManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/SceneManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/BodyManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/EventManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/InputManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/MusicManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/SoundManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/CameraManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/TimerManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/UIManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/GameManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Collisions/GameGridCollisions.cpp
)

set(LINK_LIBRARIES
    ${CMAKE_SOURCE_DIR}/include/WandEngine/liballegro_monolith.dll.a
    ole32
    uuid
    sqlite3
)

set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
file(WRITE ${ICON_FILE} "IDI_ICON1 ICON \"${EXEC_ICON}\"\n")

if (DEFINED CUSTOM_BUILD_TYPE)
    message(STATUS "Using custom build type: ${CUSTOM_BUILD_TYPE}")
    if (CUSTOM_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
        set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Debug")

    elseif (CUSTOM_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -static-libgcc -static-libstdc++")
        set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Release")

    else()
        message(WARNING "Unknown build type: ${CUSTOM_BUILD_TYPE}, defaulting to Release")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -static-libgcc -static-libstdc++")
        set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Release")

    endif()
else()
    message(STATUS "No CUSTOM_BUILD_TYPE specified, defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Debug")

endif()

add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${GAME_SOURCE_FILES} ${ICON_FILE})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIR}"
    OUTPUT_NAME ${EXEC_NAME}
)

if (CUSTOM_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()


add_custom_target(run
    COMMAND "${RUNTIME_OUTPUT_DIR}/${EXEC_NAME}.exe"
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY "${RUNTIME_OUTPUT_DIR}"
    COMMENT "Running the executable"
)
