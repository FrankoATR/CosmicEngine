set(COSMIC_PROJECT_NAME "PuzzleRPG")
set(COSMIC_PROJECT_MODE GAME_2D_CONFIGURATION)
set(COSMIC_PROJECT_FIRST_SCENE_HEADER "scenes/main_menu_scene.hpp")
set(COSMIC_PROJECT_FIRST_SCENE_CLASS "PuzzleRPG::MainMenuScene")
set(COSMIC_PROJECT_EXECUTABLE_NAME "PuzzleRPG")
set(COSMIC_PROJECT_WINDOW_TITLE "PuzzleRPG")
set(COSMIC_PROJECT_WINDOW_ICON_PATH "assets/textures/icon/icon.png")
set(COSMIC_PROJECT_PROGRAM_ICON_PATH "assets/textures/icon/icon.ico")
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
    systems/grid.cpp
    systems/level_data.cpp
    systems/level_runtime.cpp
    systems/dialog_box.cpp
    systems/net_protocol.cpp
    entities/puzzle_tile.cpp
    entities/puzzle_player.cpp
    entities/pickup_object.cpp
    entities/pressure_plate.cpp
    entities/button_object.cpp
    entities/pushable_object.cpp
    entities/door_object.cpp
    entities/teleporter_object.cpp
    entities/wall_block_object.cpp
    entities/torch_light_object.cpp
    entities/firefly_light_object.cpp
    entities/sign_object.cpp
    entities/decor_object.cpp
    entities/collision_block.cpp
    scenes/main_menu_scene.cpp
    scenes/multiplayer_lobby_scene.cpp
    scenes/level_select_scene.cpp
    scenes/gameplay_scene.cpp
    scenes/editor_select_scene.cpp
    scenes/editor_scene.cpp
    systems/menu_background.cpp
)
