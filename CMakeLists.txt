cmake_minimum_required(VERSION 3.10)

project(WandEngine)

if(NOT DEFINED PROGRAM_NAME)
    set(PROGRAM_NAME "Program")
endif()

set(EXEC_NAME ${PROGRAM_NAME})
message(STATUS "Executable name set to: ${EXEC_NAME}")

set(EXEC_ICON ${CMAKE_SOURCE_DIR}/assets/icon.ico)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(GAME_SOURCE_FILES
    #${CMAKE_SOURCE_DIR}/src/Scenes/MainScene.cpp
    #${CMAKE_SOURCE_DIR}/src/Entities/Player.cpp
    #${CMAKE_SOURCE_DIR}/src/Entities/SolidBlock.cpp
    #${CMAKE_SOURCE_DIR}/src/Entities/Spike.cpp
    #${CMAKE_SOURCE_DIR}/src/Entities/Orb.cpp
    ${CMAKE_SOURCE_DIR}/src/Scenes/SecondScene3D.cpp
    ${CMAKE_SOURCE_DIR}/src/Entities/Cube.cpp
)

set(ENGINE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/include/stb_image.cpp

    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Mesh/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Model/Model.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Shader/Shader.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Texture/GameTexture2D.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/TextFont/TextFont.cpp

    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Scene/Scene.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Object/Object.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Body/Body.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Timer/Timer.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/Light/Light.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UI/UIElement.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UI/Derived/UIButton.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Models/UI/Derived/UIText.cpp

    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/GameManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Resource/ResourceManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Object/ObjectManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Scene/SceneManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Body/BodyManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Event/EventManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Input/InputManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Audio/Music/MusicManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Audio/Sound/SoundManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Camera/CameraManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Timer/TimerManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/UI/UIManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/DataBase/DataBaseManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Light/LightManager.cpp
    ${CMAKE_SOURCE_DIR}/include/WandEngine/Managers/Network/NetworkManager.cpp

    ${CMAKE_SOURCE_DIR}/include/WandEngine/Collisions/GameGridCollisions.cpp
)

set(IMGUI_FILES
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_impl_opengl3.cpp
)

set(LINK_LIBRARIES
    ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
    ${CMAKE_SOURCE_DIR}/lib/libfreetype.a
    ${CMAKE_SOURCE_DIR}/lib/libglm.a
    ${CMAKE_SOURCE_DIR}/lib/libassimp.a
    ${CMAKE_SOURCE_DIR}/lib/libzlibstatic.a
    ${CMAKE_SOURCE_DIR}/lib/FMOD/fmod_vc.lib
    ${CMAKE_SOURCE_DIR}/lib/FMOD/fmodL_vc.lib
    ${CMAKE_SOURCE_DIR}/lib/FMOD/fmodstudio_vc.lib
    ${CMAKE_SOURCE_DIR}/lib/FMOD/fmodstudioL_vc.lib
    ${CMAKE_SOURCE_DIR}/lib/libenet.a
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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -static -static-libgcc -static-libstdc++ -pthread")
        set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Release")

    else()
        message(WARNING "Unknown build type: ${CUSTOM_BUILD_TYPE}, defaulting to Release")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -static -static-libgcc -static-libstdc++ -pthread")
        set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Release")

    endif()
else()
    message(STATUS "No CUSTOM_BUILD_TYPE specified, defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Debug")

endif()

add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${GAME_SOURCE_FILES} ${IMGUI_FILES} ${ICON_FILE})

target_compile_definitions(${PROJECT_NAME} PRIVATE ASIO_STANDALONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES} ws2_32 mswsock winmm)
endif()



set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIR}"
    OUTPUT_NAME ${EXEC_NAME}
)

if (CUSTOM_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
endif ()


add_custom_target(run
    COMMAND "${RUNTIME_OUTPUT_DIR}/${EXEC_NAME}.exe"
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY "${RUNTIME_OUTPUT_DIR}"
    COMMENT "Running the executable"
)


file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/engine/*")

# Objetivo para copiar assets
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNTIME_OUTPUT_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${RUNTIME_OUTPUT_DIR}/assets"
    DEPENDS ${ASSET_FILES}
)

add_dependencies(${PROJECT_NAME} copy_assets)
