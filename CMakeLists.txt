cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Cosmic_Engine LANGUAGES C CXX)


#**************************************BEGIN USER VARIABLES**************************************#
set(PROGRAM_NAME_BASE "COSMIC ENGINE")

set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")

#Change betwen DEBUG and RELEASE

set(GAME_MODE_CONFIGURATION GAME_3D_CONFIGURATION CACHE STRING "Selecciona el modo del juego: GAME_2D_CONFIGURATION o GAME_3D_CONFIGURATION")

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

set(CONTROLLER_SOURCE_FILES)

if(GAME_MODE_CONFIGURATION STREQUAL "GAME_2D_CONFIGURATION")
    list(APPEND GAME_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/entities/json_demo_object.cpp
    )
    list(APPEND CONTROLLER_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/controllers/camera/classic_2d_camera_controller.cpp
    )
elseif(GAME_MODE_CONFIGURATION STREQUAL "GAME_3D_CONFIGURATION")
    list(APPEND GAME_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/entities/collision_demo_object_3d.cpp
        ${CMAKE_SOURCE_DIR}/src/entities/network_demo_player.cpp
        ${CMAKE_SOURCE_DIR}/src/entities/network_demo_projectile.cpp
        ${CMAKE_SOURCE_DIR}/src/scenes/network_demo_scene.cpp
    )
    list(APPEND CONTROLLER_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/controllers/camera/classic_3d_camera_controller.cpp
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
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/animation/animation_clip.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/animation/animation_player.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/light/light.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/ui_element.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/derived/ui_button.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/derived/ui_text.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/models/ui/derived/ui_text_field.cpp

    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/game_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/resource/resource_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/object/object_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/scene/scene_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/body/body_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/event/event_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/input/input_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/audio/audio_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/animation/animation_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/camera/camera_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/timer/timer_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/CosmicEngine/managers/schedule/schedule_manager.cpp
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

set(WINDOWS_LINK_LIBRARIES
    ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
    ${CMAKE_SOURCE_DIR}/lib/libfreetype.a
    ${CMAKE_SOURCE_DIR}/lib/libglm.a
    ${CMAKE_SOURCE_DIR}/lib/libassimp.a
    ${CMAKE_SOURCE_DIR}/lib/libzlibstatic.a
    ${CMAKE_SOURCE_DIR}/lib/libenet.a
    ole32
    uuid
)




if (WIN32)
    set(PLATFORM_OUTPUT_SUBDIR "windows")
else()
    set(PLATFORM_OUTPUT_SUBDIR "linux")
endif()

set(RUNTIME_OUTPUT_BASE_DIR "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_OUTPUT_SUBDIR}")
set(RUNTIME_OUTPUT_DIR_DEBUG "${RUNTIME_OUTPUT_BASE_DIR}/Debug")
set(RUNTIME_OUTPUT_DIR_RELEASE "${RUNTIME_OUTPUT_BASE_DIR}/Release")
set(RUNTIME_OUTPUT_DIR_MINSIZEREL "${RUNTIME_OUTPUT_BASE_DIR}/MinSizeRel")
set(RUNTIME_OUTPUT_DIR_RELWITHDEBINFO "${RUNTIME_OUTPUT_BASE_DIR}/RelWithDebInfo")



set(SQLITE3_DIR "${CMAKE_SOURCE_DIR}/include/sqlite")

set(PLATFORM_SOURCE_FILES)
set(RUNTIME_DEPENDENCY_DLLS)
set(PROJECT_RUNTIME_DLLS)

if (WIN32)
    set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
    file(WRITE ${ICON_FILE} "IDI_ICON1 ICON \"${PROGRAM_ICON}\"\n")
    list(APPEND PLATFORM_SOURCE_FILES ${ICON_FILE})

    get_filename_component(MINGW_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    if (MINGW)
        list(APPEND RUNTIME_DEPENDENCY_DLLS
            "${MINGW_RUNTIME_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_RUNTIME_DIR}/libstdc++-6.dll"
            "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll"
        )
    endif()

    file(GLOB PROJECT_RUNTIME_DLLS "${CMAKE_SOURCE_DIR}/dll/*.dll")
else()
    find_package(OpenGL REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(Threads REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(assimp REQUIRED)

    pkg_check_modules(GLFW3 REQUIRED glfw3)

    list(APPEND PLATFORM_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/enet_impl.c
    )
endif()

set(BUNDLE_LINUX_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/BundleLinuxDeps.cmake")
set(WRITE_LINUX_LAUNCHER_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/WriteLinuxLauncher.cmake")

add_library(sqlite3 STATIC
    ${SQLITE3_DIR}/sqlite3.c
)

target_include_directories(sqlite3 PUBLIC
    ${SQLITE3_DIR}
)

add_executable(
        ${PROJECT_NAME} 
        ${ENGINE_SOURCE_FILES} 
    ${CONTROLLER_SOURCE_FILES}
        ${GAME_SOURCE_FILES} 
        ${IMGUI_FILES} 
        ${PLATFORM_SOURCE_FILES}
    )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

if (NOT WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLFW3_INCLUDE_DIRS}
    )

    if (ASSIMP_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS})
    endif()

    target_link_directories(${PROJECT_NAME} PRIVATE
        ${GLFW3_LIBRARY_DIRS}
    )
endif()


# Define constantes numéricas para comparar en C++
target_compile_definitions(${PROJECT_NAME} PRIVATE
    GAME_2D_CONFIGURATION=0
    GAME_3D_CONFIGURATION=1
)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
endif()

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
    target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDOWS_LINK_LIBRARIES} sqlite3 ws2_32 mswsock winmm)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        sqlite3
        OpenGL::GL
        Freetype::Freetype
        ZLIB::ZLIB
        Threads::Threads
        ${GLFW3_LIBRARIES}
    )

    if (TARGET assimp::assimp)
        target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${ASSIMP_LIBRARIES})
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE dl m)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RUNTIME_OUTPUT_DIR_DEBUG}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_OUTPUT_DIR_RELEASE}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${RUNTIME_OUTPUT_DIR_MINSIZEREL}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${RUNTIME_OUTPUT_DIR_RELWITHDEBINFO}"
    OUTPUT_NAME_DEBUG "${PROGRAM_NAME_BASE} DEBUG"
    OUTPUT_NAME_RELEASE "${PROGRAM_NAME_BASE}"
    OUTPUT_NAME_MINSIZEREL "${PROGRAM_NAME_BASE}"
    OUTPUT_NAME_RELWITHDEBINFO "${PROGRAM_NAME_BASE} DEBUG"
)

if (NOT WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "\$ORIGIN/lib"
        INSTALL_RPATH "\$ORIGIN/lib"
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/imgui.ini" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imgui.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/json_demo_save.json" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/json_demo_save.json"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/logs"
)

if (WIN32)
    foreach(RUNTIME_DLL ${RUNTIME_DEPENDENCY_DLLS} ${PROJECT_RUNTIME_DLLS})
        if (EXISTS "${RUNTIME_DLL}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${RUNTIME_DLL}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            )
        endif()
    endforeach()

    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/lib"
        COMMAND ${CMAKE_COMMAND}
            -DTARGET_FILE=$<TARGET_FILE:${PROJECT_NAME}>
            -DTARGET_DIR=$<TARGET_FILE_DIR:${PROJECT_NAME}>
            -P "${BUNDLE_LINUX_SCRIPT}"
        COMMAND ${CMAKE_COMMAND}
            -DTARGET_FILE_NAME=$<TARGET_FILE_NAME:${PROJECT_NAME}>
            -DTARGET_DIR=$<TARGET_FILE_DIR:${PROJECT_NAME}>
            -P "${WRITE_LINUX_LAUNCHER_SCRIPT}"
    )
endif()


add_custom_target(run
    COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Running the executable"
)

# ── Doxygen documentation target ──────────────────────────────────────
find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating Doxygen documentation"
    )
endif()

