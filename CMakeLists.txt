cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#**************************************BEGIN USER VARIABLES**************************************#
project(Cosmic_Engine)

set(PROGRAM_NAME "COSMIC ENGINE")

set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")

#Change betwen DEBUG and RELEASE

set(GAME_MODE_CONFIGURATION GAME_2D_CONFIGURATION CACHE STRING "Selecciona el modo del juego: GAME_2D_CONFIGURATION o GAME_3D_CONFIGURATION")

set(GAME_INITIAL_SCREEN_WIDTH 1920 CACHE STRING "Default game window width")
set(GAME_INITIAL_SCREEN_HEIGHT 1080 CACHE STRING "Default game window height")

#RENDER ASPECT, CURRENT BASE: 16:9   ->   1920/1080
set(GAME_BASE_RENDER_SCREEN_WIDTH 1920)
set(GAME_BASE_RENDER_SCREEN_HEIGHT 1080)

#Change betwen DEBUG and RELEASE
set(GAME_FIRST_SCENE
    ${CMAKE_SOURCE_DIR}/src/scenes/main_scene.cpp
)

set(GAME_SOURCE_FILES
    ${GAME_FIRST_SCENE}
)

if(GAME_MODE_CONFIGURATION STREQUAL "GAME_2D_CONFIGURATION")
    list(APPEND GAME_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/entities/json_demo_object.cpp
    )
endif()


#**************************************END USER VARIABLES**************************************#


include_directories(${CMAKE_SOURCE_DIR}/include)


set(ENGINE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/include/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/include/miniaudio/miniaudio.c

    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/mesh/mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/model/model.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/shader/shader.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/texture/texture_2d.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/font/font.cpp

    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/scene/scene.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/object/object.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/body/body.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/timer/timer.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/light/light.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/ui_element.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/derived/ui_button.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/derived/ui_text.cpp

    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/game_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/resource/resource_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/object/object_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/scene/scene_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/body/body_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/event/event_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/input/input_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/audio/audio_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/camera/camera_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/timer/timer_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/ui/ui_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/storage/sql/sql_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/storage/json/json_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/light/light_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/network/network_manager.cpp

    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/GameGridCollisions.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/CollisionArea.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/GameQuadTreeCollisions.cpp
    
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/utils/logger.cpp

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
    ${CMAKE_SOURCE_DIR}/lib/libenet.a
    ole32
    uuid
)




if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROGRAM_NAME "${PROGRAM_NAME} DEBUG")
    set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")
    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Debug")

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -static -static-libgcc -static-libstdc++ -pthread")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Release")

elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/MinSizeRel")

elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(PROGRAM_NAME "${PROGRAM_NAME} DEBUG")
    set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/RelWithDebInfo")
    
else()
    message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}, defaulting to Debug")
    set(PROGRAM_NAME "${PROGRAM_NAME} DEBUG")
    set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")
    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(RUNTIME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/Debug")
endif()



set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
file(WRITE ${ICON_FILE} "IDI_ICON1 ICON \"${PROGRAM_ICON}\"\n")


set(SQLITE3_DIR "${CMAKE_SOURCE_DIR}/include/sqlite")

add_library(sqlite3 STATIC
    ${SQLITE3_DIR}/sqlite3.c
)

target_include_directories(sqlite3 PUBLIC
    ${SQLITE3_DIR}
)

add_executable(
        ${PROJECT_NAME} 
        ${ENGINE_SOURCE_FILES} 
        ${GAME_SOURCE_FILES} 
        ${IMGUI_FILES} 
        ${ICON_FILE}
    )


# Define constantes numéricas para comparar en C++
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _WIN32_WINNT=0x0A00
    GAME_2D_CONFIGURATION=0
    GAME_3D_CONFIGURATION=1
)

# Validación del modo seleccionado (2D o 3D)
if(GAME_MODE_CONFIGURATION STREQUAL "GAME_3D_CONFIGURATION")
    message(STATUS "Active mode: 3D")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_MODE_CONFIGURATION=GAME_3D_CONFIGURATION)
elseif(GAME_MODE_CONFIGURATION STREQUAL "GAME_2D_CONFIGURATION")
    message(STATUS "Active mode: 2D")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_MODE_CONFIGURATION=GAME_2D_CONFIGURATION)
else()
    message(FATAL_ERROR "GAME_MODE_CONFIGURATION must be 'GAME_2D_CONFIGURATION' o 'GAME_3D_CONFIGURATION'")
endif()

# Propagar resoluciones como defines al compilador
target_compile_definitions(${PROJECT_NAME} PRIVATE
    GAME_INITIAL_SCREEN_WIDTH=${GAME_INITIAL_SCREEN_WIDTH}
    GAME_INITIAL_SCREEN_HEIGHT=${GAME_INITIAL_SCREEN_HEIGHT}
    GAME_BASE_RENDER_SCREEN_WIDTH=${GAME_BASE_RENDER_SCREEN_WIDTH}
    GAME_BASE_RENDER_SCREEN_HEIGHT=${GAME_BASE_RENDER_SCREEN_HEIGHT}
)


if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES} ws2_32 mswsock winmm)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIR}"
    OUTPUT_NAME ${PROGRAM_NAME}
)

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()


add_custom_target(run
    COMMAND "${RUNTIME_OUTPUT_DIR}/${PROGRAM_NAME}.exe"
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
