#ifndef PUZZLERPG_DECOR_OBJECT_HPP
#define PUZZLERPG_DECOR_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    // Adorno visual puro: se dibuja en su celda pero no tiene colision ni logica.
    class DecorObject : public CosmicEngine::Object
    {
    public:
        DecorObject(int id, int spriteSheet, int spriteRow, int spriteCol, Cell cell);

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell()     const { return cell; }

    private:
        int  objectId;
        Cell cell;
        int  spriteSheet;
        int  spriteRow;
        int  spriteCol;
    };
}

#endif
