#ifndef PUZZLERPG_PUZZLE_TILE_HPP
#define PUZZLERPG_PUZZLE_TILE_HPP

#include "../systems/level_data.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    // Tile estatico de una sola capa de suelo / muro. Cada TileLayer se
    // registra como un Object independiente para que el ObjectManager pueda
    // intercalar correctamente su drawLayer con jugadores y otros objetos.
    class PuzzleTile : public CosmicEngine::Object
    {
    public:
        PuzzleTile(TileLayer layer, int gridX, int gridY);

        void draw() const override;

        const TileLayer &GetTileLayer() const { return tileLayer; }
        int       GetGridX() const { return gridX; }
        int       GetGridY() const { return gridY; }

    private:
        TileLayer tileLayer;
        int gridX;
        int gridY;
    };
}

#endif
