#ifndef PUZZLERPG_ASSET_DEFINES_HPP
#define PUZZLERPG_ASSET_DEFINES_HPP

#include "paths.hpp"

// Rutas, keys y nombres de clips centralizados para mantener el proyecto
// legible y facil de extender cuando se agreguen spritesheets reales.

// --- Suelo: ground-1x1.png (15 filas x 13 columnas). El editor permite
// elegir cualquier celda como sprite de suelo para esa casilla.
#define PUZZLERPG_SHEET_TILE_FLOOR_KEY    "puzzlerpg.sheet.ground"

// --- Muros 1x1: walls-1x1.png (7 filas x 5 columnas). Cada celda ocupa
// exactamente una celda del grid y se trata como no transitable.
#define PUZZLERPG_SHEET_TILE_WALL_KEY     "puzzlerpg.sheet.walls1x1"

// --- Muros 2x2: walls-2x2.png (6 filas x 4 columnas). Cada "celda" de la
// hoja representa un bloque que cubre 2x2 celdas del grid (objeto multi-celda).
#define PUZZLERPG_SHEET_TILE_WALL_2X2_KEY  "puzzlerpg.sheet.walls2x2"

// --- Muros 3x3: walls-3x3.png (3 filas x 3 columnas). Cada celda cubre
// 3x3 casillas del grid.
#define PUZZLERPG_SHEET_TILE_WALL_3X3_KEY  "puzzlerpg.sheet.walls3x3"

#define PUZZLERPG_SHEET_PLAYER_KEY        "puzzlerpg.sheet.player"

// --- Pickable items: pickable-items-1x1.png (2 filas x 3 columnas).
#define PUZZLERPG_SHEET_PICKUP_KEY        "puzzlerpg.sheet.pickable"

// --- Placa de presion: pressurae-items-1x1.png (1 fila x 2 columnas) off/on.
#define PUZZLERPG_SHEET_PLATE_KEY         "puzzlerpg.sheet.pressure"

// --- Altar / receptaculo: altar-items-1x1.png (1 fila x 2 columnas) off/on.
#define PUZZLERPG_SHEET_RECEPTACLE_KEY    "puzzlerpg.sheet.altar"

#define PUZZLERPG_SHEET_DOOR_KEY          "puzzlerpg.sheet.door"

#define PUZZLERPG_SHEET_BUTTON_KEY        "puzzlerpg.sheet.button"

// --- Empujables: pushable-items-1x1.png (2 filas x 3 columnas).
#define PUZZLERPG_SHEET_PUSHABLE_KEY      "puzzlerpg.sheet.pushable"

#define PUZZLERPG_SHEET_TELEPORTER_KEY    "puzzlerpg.sheet.teleporter"

#define PUZZLERPG_SHEET_SIGN_KEY          "puzzlerpg.sheet.sign"
#define PUZZLERPG_SHEET_TORCH_KEY         "puzzlerpg.sheet.torch"
#define PUZZLERPG_SHEET_DECOR_KEY         "puzzlerpg.sheet.decor"

#define PUZZLERPG_MUSIC_MENU_KEY          "puzzlerpg.music.menu"
#define PUZZLERPG_MUSIC_MENU_PATH         PUZZLERPG_MUSIC_DIR "/menu.mp3"

#define PUZZLERPG_MUSIC_GAMEPLAY_KEY      "puzzlerpg.music.gameplay"
#define PUZZLERPG_MUSIC_GAMEPLAY_PATH     PUZZLERPG_MUSIC_DIR "/gameplay.mp3"

#define PUZZLERPG_CLIP_TILE_FLOOR         "puzzlerpg.clip.tile.floor"
#define PUZZLERPG_CLIP_TILE_WALL          "puzzlerpg.clip.tile.wall"
#define PUZZLERPG_CLIP_PLAYER_DOWN_IDLE   "puzzlerpg.clip.player.down.idle"
#define PUZZLERPG_CLIP_PLAYER_UP_IDLE     "puzzlerpg.clip.player.up.idle"
#define PUZZLERPG_CLIP_PLAYER_LEFT_IDLE   "puzzlerpg.clip.player.left.idle"
#define PUZZLERPG_CLIP_PLAYER_RIGHT_IDLE  "puzzlerpg.clip.player.right.idle"
#define PUZZLERPG_CLIP_PLAYER_DOWN_WALK   "puzzlerpg.clip.player.down.walk"
#define PUZZLERPG_CLIP_PLAYER_UP_WALK     "puzzlerpg.clip.player.up.walk"
#define PUZZLERPG_CLIP_PLAYER_LEFT_WALK   "puzzlerpg.clip.player.left.walk"
#define PUZZLERPG_CLIP_PLAYER_RIGHT_WALK  "puzzlerpg.clip.player.right.walk"
#define PUZZLERPG_CLIP_PICKUP_IDLE        "puzzlerpg.clip.pickup.idle"
#define PUZZLERPG_CLIP_PLATE_OFF          "puzzlerpg.clip.plate.off"
#define PUZZLERPG_CLIP_PLATE_ON           "puzzlerpg.clip.plate.on"
#define PUZZLERPG_CLIP_RECEPTACLE_OFF     "puzzlerpg.clip.receptacle.off"
#define PUZZLERPG_CLIP_RECEPTACLE_ON      "puzzlerpg.clip.receptacle.on"
#define PUZZLERPG_CLIP_DOOR_CLOSED        "puzzlerpg.clip.door.closed"
#define PUZZLERPG_CLIP_DOOR_OPEN          "puzzlerpg.clip.door.open"
#define PUZZLERPG_CLIP_BUTTON_OFF         "puzzlerpg.clip.button.off"
#define PUZZLERPG_CLIP_BUTTON_ON          "puzzlerpg.clip.button.on"
#define PUZZLERPG_CLIP_PUSHABLE_IDLE      "puzzlerpg.clip.pushable.idle"
#define PUZZLERPG_CLIP_TELEPORTER_IDLE    "puzzlerpg.clip.teleporter.idle"
#define PUZZLERPG_CLIP_SIGN_IDLE          "puzzlerpg.clip.sign.idle"
#define PUZZLERPG_CLIP_TORCH_IDLE         "puzzlerpg.clip.torch.idle"

#endif