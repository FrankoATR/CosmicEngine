set(COSMIC_PROJECT_NAME "NaBazu")
set(COSMIC_PROJECT_MODE GAME_3D_CONFIGURATION)
set(COSMIC_PROJECT_FIRST_SCENE_HEADER "scenes/main_menu_scene.hpp")
set(COSMIC_PROJECT_FIRST_SCENE_CLASS "NaBazu::MainMenuScene")
set(COSMIC_PROJECT_EXECUTABLE_NAME "NaBazu")
set(COSMIC_PROJECT_WINDOW_TITLE "Na bázu")
set(COSMIC_PROJECT_INITIAL_SCREEN_WIDTH 1280)
set(COSMIC_PROJECT_INITIAL_SCREEN_HEIGHT 720)
set(COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH 1920)
set(COSMIC_PROJECT_BASE_RENDER_SCREEN_HEIGHT 1080)
set(COSMIC_PROJECT_VIEWPORT_SCALE_TO_WINDOW ON)
set(COSMIC_PROJECT_WINDOW_RESIZABLE ON)
set(COSMIC_PROJECT_WINDOW_LOCK_BASE_RENDER_ASPECT OFF)
set(COSMIC_PROJECT_START_FULLSCREEN OFF)
set(COSMIC_PROJECT_START_VSYNC ON)
set(COSMIC_PROJECT_ASSET_DIR "assets")

set(COSMIC_PROJECT_SOURCE_FILES
    scenes/main_menu_scene.cpp
    scenes/gameplay_scene.cpp
    scenes/game_over_scene.cpp
    scenes/credits_scene.cpp
    scenes/game_info_scene.cpp
    systems/rail_path.cpp
    systems/wave_spawner.cpp
    systems/chase_camera.cpp
    systems/level_director.cpp
    systems/skybox.cpp
    systems/test_mode.cpp
    systems/glow_mesh_renderer.cpp
    ui/ui_gauge_bar.cpp
    entities/ship.cpp
    entities/bullet.cpp
    entities/missile.cpp
    entities/enemy_grunt.cpp
    entities/enemy_bullet.cpp
    entities/pickup.cpp
    entities/explosion_effect.cpp
)
