cmake_minimum_required(VERSION 3.10)

project(EngineTest)

set(EXEC_NAME 
    "Geometry Dash Test"
)

set(EXEC_ICON
    ${CMAKE_SOURCE_DIR}/assets/geometrydash.ico
)

set(GAME_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/Scenes/MainScene.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Player.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/SolidBlock.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Spike.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Ground.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Background.cpp
)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static-libgcc -static-libstdc++")

include_directories(${CMAKE_SOURCE_DIR}/include)

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

add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${GAME_SOURCE_FILES} ${ICON_FILE})

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})


set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    OUTPUT_NAME ${EXEC_NAME}
    WIN32_EXECUTABLE TRUE
)