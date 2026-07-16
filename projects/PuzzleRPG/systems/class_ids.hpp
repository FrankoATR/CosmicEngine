#ifndef PUZZLERPG_CLASS_IDS_HPP
#define PUZZLERPG_CLASS_IDS_HPP

// Identificadores de clase usados en el formato JSON de niveles y en el
// protocolo de red. Son enteros estables que no dependen de nombres de C++.
// Se mantienen en una sola tabla para que el editor, el cargador y el runtime
// hablen el mismo idioma.
namespace PuzzleRPG
{
    enum ClassId : int
    {
        Class_None          = 0,
        Class_Pickup        = 1,
        Class_PressurePlate = 2,
        Class_Door          = 3,
        Class_Button        = 4,
        Class_Pushable      = 5,
        Class_PlayerSpawn   = 6,
        Class_Teleporter    = 7,  // Par emparejado por pairId.
        Class_Wall2x2       = 8,  // Bloque de muro 2x2 (objeto multi-celda).
        Class_Wall3x3       = 9,  // Bloque de muro 3x3 (objeto multi-celda).
        Class_Torch         = 10,
        Class_Firefly       = 11,
        Class_Sign          = 12,
        Class_Decor         = 13,  // Adorno visual, sin colision, 1x1.
        Class_Block         = 14   // Bloque de colision invisible, 1x1.
    };

    // Para clases de muro multi-celda devuelve el lado (en celdas).
    // 0 si la clase no es un muro multi-celda.
    inline int WallObjectSpan(int classId)
    {
        if (classId == Class_Wall2x2) return 2;
        if (classId == Class_Wall3x3) return 3;
        return 0;
    }

    // Modo de activacion de las placas de presion.
    enum PlateActivator : int
    {
        Plate_Player        = 1,
        Plate_Object        = 2,
        Plate_Both          = 3,
        Plate_SpecificItem  = 4   // Solo activa con un Pickup de textureId concreto.
    };

    // Identificadores de hoja de sprites (spritesheet) usados por los tiles.
    // Cada celda de `LevelData.tiles` referencia una hoja por id, y dentro de
    // esa hoja una `(row, col)`. Los ids son estables porque se persisten en
    // los JSON de nivel.
    namespace SheetId
    {
        constexpr int kNone     = 0;  // Celda vacia / sin sprite.
        constexpr int kGround   = 1;  // ground-1x1.png    (15 filas x 13 columnas)
        constexpr int kWalls1x1 = 2;  // walls-1x1.png     ( 7 filas x  5 columnas)
        constexpr int kWalls2x2 = 3;  // walls-2x2.png     ( 6 filas x  4 columnas)
        constexpr int kWalls3x3 = 4;  // walls-3x3.png     ( 3 filas x  3 columnas)
        constexpr int kPushable = 5;  // pushable-items-1x1.png ( 2 filas x  3 columnas)
        constexpr int kPickable = 6;  // pickable-items-1x1.png ( 2 filas x  3 columnas)
        constexpr int kPressure = 7;  // pressure-items-1x1.png ( 1 fila  x  2 columnas) off/on
        constexpr int kAltar    = 8;  // altar-items-1x1.png    ( 1 fila  x  2 columnas) off/on
        constexpr int kPlayer   = 9;  // player_sheet.png  ( 4 filas x  4 columnas)
        constexpr int kDecor    = 10; // ground-items-1x1.png     (spritesheets de adornos)
    }

    // Devuelve true si el id de hoja corresponde a alguna variante de muro.
    // Los tiles que apuntan a una hoja de muros se consideran no transitables.
    inline bool IsWallSheet(int sheetId)
    {
        return sheetId == SheetId::kWalls1x1
            || sheetId == SheetId::kWalls2x2
            || sheetId == SheetId::kWalls3x3;
    }

    inline int DefaultPlacementLayerForClass(int)
    {
        return 0;
    }

    inline int DefaultPlacementLayerForSheet(int)
    {
        return 0;
    }

    inline int DefaultDrawLayerForSheet(int sheetId)
    {
        return IsWallSheet(sheetId) ? 1 : 0;
    }

    inline int DefaultDrawLayerForClass(int classId)
    {
        switch (classId)
        {
            case Class_PlayerSpawn:
            case Class_PressurePlate:
            case Class_Teleporter:
            case Class_Wall2x2:
            case Class_Wall3x3:
                return 1;
            case Class_Torch:
            case Class_Firefly:
                return 2;
            case Class_Decor:
            case Class_Block:
                return 1;
            default:
                return 2;
        }
    }

    // Ids de textura por defecto que el editor sugiere al crear cada clase.
    // El usuario puede sobreescribirlos en el JSON. Son simples enteros que
    // mas adelante se mapearan a archivos PNG reales.
    namespace DefaultTextureId
    {
        constexpr int kFloor           = 1;
        constexpr int kWall            = 0;
        constexpr int kPlayerDown      = 50;
        constexpr int kPlayerUp        = 51;
        constexpr int kPlayerLeft      = 52;
        constexpr int kPlayerRight     = 53;
        constexpr int kPickup          = 100;
        constexpr int kPressurePlate   = 110;
        constexpr int kPressurePlateOn = 111;
        constexpr int kDoorClosed      = 120;
        constexpr int kDoorOpen        = 121;
        constexpr int kButtonOff       = 130;
        constexpr int kButtonOn        = 131;
        constexpr int kPushable        = 140;
        constexpr int kTeleporter      = 150;
        constexpr int kReceptacle      = 160;
    }

    // Convenciones:
    //  - tileId == 0    : no transitable (muro / vacio)
    //  - tileId >= 1    : suelo transitable
    //  - una puerta cerrada bloquea la casilla
    //  - un objeto Pushable bloquea la casilla del jugador hasta empujarlo
    //  - un Pickup no bloquea la casilla; puede ser recogido
}

#endif
