cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Cosmic_Engine LANGUAGES C CXX)


#**************************************BEGIN USER VARIABLES**************************************#
set(PROGRAM_NAME_BASE "COSMIC ENGINE")

set(PROGRAM_ICON "${CMAKE_SOURCE_DIR}/default/icon.ico")

set(COSMIC_PROJECT "sandbox_3D" CACHE STRING "Project folder under projects/ to configure and build")

set(GAME_INITIAL_SCREEN_WIDTH 1920 CACHE STRING "Default game window width")
set(GAME_INITIAL_SCREEN_HEIGHT 1080 CACHE STRING "Default game window height")

#RENDER ASPECT, CURRENT BASE: 16:9   ->   1920/1080
set(GAME_BASE_RENDER_SCREEN_WIDTH 1920)
set(GAME_BASE_RENDER_SCREEN_HEIGHT 1080)

set(COSMIC_PROJECTS_DIR "${CMAKE_SOURCE_DIR}/projects")
file(GLOB COSMIC_PROJECT_CANDIDATES RELATIVE "${COSMIC_PROJECTS_DIR}" "${COSMIC_PROJECTS_DIR}/*")
set(COSMIC_AVAILABLE_PROJECTS)
foreach(candidate ${COSMIC_PROJECT_CANDIDATES})
    if(IS_DIRECTORY "${COSMIC_PROJECTS_DIR}/${candidate}")
        list(APPEND COSMIC_AVAILABLE_PROJECTS "${candidate}")
    endif()
endforeach()

set_property(CACHE COSMIC_PROJECT PROPERTY STRINGS ${COSMIC_AVAILABLE_PROJECTS})

set(COSMIC_PROJECT_DIR "${COSMIC_PROJECTS_DIR}/${COSMIC_PROJECT}")
if(NOT EXISTS "${COSMIC_PROJECT_DIR}/project.cmake")
    string(REPLACE ";" ", " COSMIC_AVAILABLE_PROJECTS_STR "${COSMIC_AVAILABLE_PROJECTS}")
    message(FATAL_ERROR "COSMIC_PROJECT='${COSMIC_PROJECT}' is invalid. Expected one of: ${COSMIC_AVAILABLE_PROJECTS_STR}")
endif()

include("${COSMIC_PROJECT_DIR}/project.cmake")

if(NOT DEFINED COSMIC_PROJECT_NAME)
    set(COSMIC_PROJECT_NAME "${COSMIC_PROJECT}")
endif()

if(NOT DEFINED COSMIC_PROJECT_EXECUTABLE_NAME)
    set(COSMIC_PROJECT_EXECUTABLE_NAME "${PROGRAM_NAME_BASE}")
endif()

if(NOT DEFINED COSMIC_PROJECT_MODE)
    message(FATAL_ERROR "Project '${COSMIC_PROJECT}' must define COSMIC_PROJECT_MODE in projects/${COSMIC_PROJECT}/project.cmake")
endif()

if(NOT DEFINED COSMIC_PROJECT_FIRST_SCENE_HEADER)
    message(FATAL_ERROR "Project '${COSMIC_PROJECT}' must define COSMIC_PROJECT_FIRST_SCENE_HEADER in projects/${COSMIC_PROJECT}/project.cmake")
endif()

if(NOT DEFINED COSMIC_PROJECT_FIRST_SCENE_CLASS)
    message(FATAL_ERROR "Project '${COSMIC_PROJECT}' must define COSMIC_PROJECT_FIRST_SCENE_CLASS in projects/${COSMIC_PROJECT}/project.cmake")
endif()

if(NOT DEFINED COSMIC_PROJECT_SOURCE_FILES)
    set(COSMIC_PROJECT_SOURCE_FILES)
endif()

if(NOT DEFINED COSMIC_PROJECT_INITIAL_SCREEN_WIDTH)
    set(COSMIC_PROJECT_INITIAL_SCREEN_WIDTH ${GAME_INITIAL_SCREEN_WIDTH})
endif()

if(NOT DEFINED COSMIC_PROJECT_INITIAL_SCREEN_HEIGHT)
    set(COSMIC_PROJECT_INITIAL_SCREEN_HEIGHT ${GAME_INITIAL_SCREEN_HEIGHT})
endif()

if(NOT DEFINED COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH)
    set(COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH ${GAME_BASE_RENDER_SCREEN_WIDTH})
endif()

if(NOT DEFINED COSMIC_PROJECT_BASE_RENDER_SCREEN_HEIGHT)
    set(COSMIC_PROJECT_BASE_RENDER_SCREEN_HEIGHT ${GAME_BASE_RENDER_SCREEN_HEIGHT})
endif()

if(NOT DEFINED COSMIC_PROJECT_START_FULLSCREEN)
    set(COSMIC_PROJECT_START_FULLSCREEN OFF)
endif()

if(NOT DEFINED COSMIC_PROJECT_START_VSYNC)
    set(COSMIC_PROJECT_START_VSYNC OFF)
endif()

if(NOT DEFINED COSMIC_PROJECT_WINDOW_TITLE)
    set(COSMIC_PROJECT_WINDOW_TITLE "${COSMIC_PROJECT_NAME}")
endif()

if(NOT DEFINED COSMIC_PROJECT_WINDOW_ICON_PATH)
    set(COSMIC_PROJECT_WINDOW_ICON_PATH "")
endif()

if(COSMIC_PROJECT_START_FULLSCREEN)
    set(COSMIC_PROJECT_START_FULLSCREEN_CPP true)
else()
    set(COSMIC_PROJECT_START_FULLSCREEN_CPP false)
endif()

if(COSMIC_PROJECT_START_VSYNC)
    set(COSMIC_PROJECT_START_VSYNC_CPP true)
else()
    set(COSMIC_PROJECT_START_VSYNC_CPP false)
endif()

if(NOT DEFINED COSMIC_PROJECT_ASSET_DIR)
    set(COSMIC_PROJECT_ASSET_DIR "assets")
endif()

function(cosmic_resolve_project_paths outputVar baseDir)
    set(resolved)
    foreach(pathEntry IN LISTS ARGN)
        if(IS_ABSOLUTE "${pathEntry}")
            list(APPEND resolved "${pathEntry}")
        else()
            list(APPEND resolved "${baseDir}/${pathEntry}")
        endif()
    endforeach()
    set(${outputVar} "${resolved}" PARENT_SCOPE)
endfunction()

cosmic_resolve_project_paths(GAME_SOURCE_FILES "${COSMIC_PROJECT_DIR}" ${COSMIC_PROJECT_SOURCE_FILES})

if(IS_ABSOLUTE "${COSMIC_PROJECT_ASSET_DIR}")
    set(COSMIC_PROJECT_ASSET_DIR_RESOLVED "${COSMIC_PROJECT_ASSET_DIR}")
else()
    set(COSMIC_PROJECT_ASSET_DIR_RESOLVED "${COSMIC_PROJECT_DIR}/${COSMIC_PROJECT_ASSET_DIR}")
endif()

file(TO_CMAKE_PATH "${COSMIC_PROJECT_ASSET_DIR_RESOLVED}" COSMIC_PROJECT_ASSET_DIR_CMAKE)

if(COSMIC_PROJECT_WINDOW_ICON_PATH STREQUAL "")
    set(COSMIC_PROJECT_WINDOW_ICON_PATH_CMAKE "")
elseif(IS_ABSOLUTE "${COSMIC_PROJECT_WINDOW_ICON_PATH}")
    set(COSMIC_PROJECT_WINDOW_ICON_PATH_RESOLVED "${COSMIC_PROJECT_WINDOW_ICON_PATH}")
else()
    set(COSMIC_PROJECT_WINDOW_ICON_PATH_RESOLVED "${COSMIC_PROJECT_DIR}/${COSMIC_PROJECT_WINDOW_ICON_PATH}")
endif()

if(DEFINED COSMIC_PROJECT_WINDOW_ICON_PATH_RESOLVED)
    file(TO_CMAKE_PATH "${COSMIC_PROJECT_WINDOW_ICON_PATH_RESOLVED}" COSMIC_PROJECT_WINDOW_ICON_PATH_CMAKE)
endif()

set(CONTROLLER_SOURCE_FILES)
if(COSMIC_PROJECT_MODE STREQUAL "GAME_2D_CONFIGURATION")
    list(APPEND CONTROLLER_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/controllers/camera/classic_2d_camera_controller.cpp
    )
elseif(COSMIC_PROJECT_MODE STREQUAL "GAME_3D_CONFIGURATION")
    list(APPEND CONTROLLER_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/controllers/camera/classic_3d_camera_controller.cpp
    )
else()
    message(FATAL_ERROR "COSMIC_PROJECT_MODE must be GAME_2D_CONFIGURATION or GAME_3D_CONFIGURATION for project '${COSMIC_PROJECT}'")
endif()

set(COSMIC_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${COSMIC_GENERATED_DIR}")
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/cosmic_project_entry.hpp.in"
    "${COSMIC_GENERATED_DIR}/cosmic_project_entry.hpp"
    @ONLY
)

configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/cosmic_project_config.hpp.in"
    "${COSMIC_GENERATED_DIR}/cosmic_project_config.hpp"
    @ONLY
)


#**************************************END USER VARIABLES**************************************#


include_directories(${CMAKE_SOURCE_DIR}/include)


set(ENGINE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/main.cpp
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

        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/game_grid_collisions.cpp
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/collision_area.cpp
        ${CMAKE_SOURCE_DIR}/include/CosmicEngine/collisions/game_quad_tree_collisions.cpp
    
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

set(RUNTIME_OUTPUT_BASE_DIR "${CMAKE_SOURCE_DIR}/bin/${COSMIC_PROJECT}/${PLATFORM_OUTPUT_SUBDIR}")
set(RUNTIME_OUTPUT_DIR_DEBUG "${RUNTIME_OUTPUT_BASE_DIR}/Debug")
set(RUNTIME_OUTPUT_DIR_RELEASE "${RUNTIME_OUTPUT_BASE_DIR}/Release")
set(RUNTIME_OUTPUT_DIR_MINSIZEREL "${RUNTIME_OUTPUT_BASE_DIR}/MinSizeRel")
set(RUNTIME_OUTPUT_DIR_RELWITHDEBINFO "${RUNTIME_OUTPUT_BASE_DIR}/RelWithDebInfo")
set(PROJECT_ASSET_OUTPUT_DIR "${RUNTIME_OUTPUT_BASE_DIR}/$<CONFIG>/assets")



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
        ${CMAKE_SOURCE_DIR}/include/enet/enet_impl.c
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

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${COSMIC_PROJECT_DIR} ${COSMIC_GENERATED_DIR})

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
if(COSMIC_PROJECT_MODE STREQUAL "GAME_3D_CONFIGURATION")
    message(STATUS "Active project: ${COSMIC_PROJECT} (3D)")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_MODE_CONFIGURATION=GAME_3D_CONFIGURATION)
elseif(COSMIC_PROJECT_MODE STREQUAL "GAME_2D_CONFIGURATION")
    message(STATUS "Active project: ${COSMIC_PROJECT} (2D)")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_MODE_CONFIGURATION=GAME_2D_CONFIGURATION)
else()
    message(FATAL_ERROR "COSMIC_PROJECT_MODE must be 'GAME_2D_CONFIGURATION' or 'GAME_3D_CONFIGURATION'")
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
    OUTPUT_NAME_DEBUG "${COSMIC_PROJECT_EXECUTABLE_NAME} DEBUG"
    OUTPUT_NAME_RELEASE "${COSMIC_PROJECT_EXECUTABLE_NAME}"
    OUTPUT_NAME_MINSIZEREL "${COSMIC_PROJECT_EXECUTABLE_NAME}"
    OUTPUT_NAME_RELWITHDEBINFO "${COSMIC_PROJECT_EXECUTABLE_NAME} DEBUG"
)

if (NOT WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "\$ORIGIN/lib"
        INSTALL_RPATH "\$ORIGIN/lib"
    )
endif()

add_custom_target(copy_project_assets
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_ASSET_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_ASSET_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${COSMIC_PROJECT_ASSET_DIR_RESOLVED}" "${PROJECT_ASSET_OUTPUT_DIR}"
    COMMENT "Copying assets for project ${COSMIC_PROJECT}"
)

add_dependencies(${PROJECT_NAME} copy_project_assets)

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

