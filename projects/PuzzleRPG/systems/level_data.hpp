#ifndef PUZZLERPG_LEVEL_DATA_HPP
#define PUZZLERPG_LEVEL_DATA_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace PuzzleRPG
{
    struct TileLayer
    {
        int sheet          = 0;
        int row            = 0;
        int col            = 0;
        int placementLayer = 0;
        int drawLayer      = 0;
    };

    struct LevelBackgroundColor
    {
        int r = 158;
        int g = 194;
        int b = 107;
    };

    // Definicion en datos de un objeto en el nivel.
    // - id      : identificador unico dentro del nivel (>=1). El 0 esta reservado.
    // - classId : ver class_ids.hpp (puerta=3, placa=2, etc.).
    // - textureId: id global de textura que mas adelante se mapeara a un PNG.
    // - x, y    : posicion en celdas dentro de la sala.
    // - targets : ids de otros objetos a los que afecta (solo plates/buttons -> doors).
    // - activator: solo placas; 1=jugador, 2=objeto, 3=ambos.
    // - facing  : solo botones; direccion en la que se debe mirar para activarlo.
    // - dialog  : texto opcional que muestra al interactuar (informativo).
    struct LevelObject
    {
        int id            = 0;
        int classId       = 0;
        int textureId     = 0;
        int x             = 0;
        int y             = 0;
        std::vector<int> targets;
        int activator     = 3;
        int facing        = 0;
        // Plate_SpecificItem: objectId del Pickup requerido para activar.
        int requiredItemTextureId = 0;
        // Class_Teleporter: id del otro teletransporte emparejado.
        int pairId        = 0;
        // Variante de sprite por instancia. sheet == 0 -> usar el por defecto
        // de la clase. Usado por pushable/pickable para elegir entre varias
        // texturas de la misma hoja, y por los muros multi-celda Wall2x2/3x3
        // para indicar que sub-celda de su hoja usar.
        int spriteSheet   = 0;
        int spriteRow     = 0;
        int spriteCol     = 0;
        int placementLayer = 0;
        int drawLayer      = 2;
        std::string dialog;
        // Pixel-exact world position for decorations placed in "Grid libre" mode.
        // When both are 0.0f the cell-based position (x * kTileSize, y * kTileSize) is used.
        float pixelX = 0.0f;
        float pixelY = 0.0f;
    };

    // Una celda ahora puede contener varias capas de tile 1x1. Cada capa ocupa
    // un placementLayer de grid y se dibuja segun drawLayer.
    struct TileCell
    {
        std::vector<TileLayer> layers;
    };

    struct LevelData
    {
        std::string name;
        int width  = 16;
        int height = 16;
        LevelBackgroundColor backgroundColor;
        // tiles[y][x] -> TileCell con la hoja y la sub-celda elegidas.
        std::vector<std::vector<TileCell>> tiles;
        // Hasta dos puntos de spawn (uno por jugador).
        std::vector<std::pair<int,int>> playerSpawns;
        std::vector<LevelObject> objects;
        // Condicion de victoria: lista de ids de activadores que deben estar
        // simultaneamente activos para terminar el nivel. Vacia = sin meta.
        std::vector<int> winActivatorIds;

        // Devuelve la TileCell de una casilla, o una TileCell vacia si esta fuera.
        TileCell TileAt(int x, int y) const;
        // Reemplaza la celda completa con la TileCell indicada.
        void SetTile(int x, int y, TileCell cell);
        // Busca la capa de tile asociada al placementLayer indicado.
        TileLayer *FindTileLayer(int x, int y, int placementLayer);
        const TileLayer *FindTileLayer(int x, int y, int placementLayer) const;
        // Inserta/reemplaza un tile layer en una celda segun placementLayer.
        void SetTileLayer(int x, int y, TileLayer layer);
        // Elimina el tile layer en la capa dada. Devuelve true si existia.
        bool RemoveTileLayer(int x, int y, int placementLayer);
        // Consultas utilitarias para editor/runtime.
        bool HasAnyTile(int x, int y) const;
        bool HasWallTile(int x, int y) const;

        // Busca un objeto por su id local. Devuelve nullptr si no existe.
        LevelObject *FindObject(int id);
        const LevelObject *FindObject(int id) const;

        // Devuelve un id libre para un nuevo objeto.
        int NextObjectId() const;
    };

    // Serializa / deserializa una sala completa.
    nlohmann::json SerializeLevel(const LevelData &level);
    bool DeserializeLevel(const nlohmann::json &data, LevelData &out);

    // Persistencia en disco. La ruta base es "assets/levels/" relativa al exe.
    std::string LevelsDirectory();
    std::string LevelFilePath(const std::string &levelName);
    bool SaveLevelToFile(const LevelData &level);
    bool LoadLevelFromFile(const std::string &levelName, LevelData &out);

    // Devuelve la lista de niveles disponibles (nombres sin extension).
    std::vector<std::string> ListAvailableLevels();

    // Crea una sala vacia con un suelo basico y un spawn.
    LevelData MakeEmptyLevel(const std::string &name, int width, int height);
}

#endif
