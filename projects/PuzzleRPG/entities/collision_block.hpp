#pragma once
#include <CosmicEngine/models/object/object.hpp>
#include "../systems/grid.hpp"

namespace PuzzleRPG
{
    // Bloque de colision 1x1 invisible en gameplay.
    // Su unica funcion es bloquear IsCellWalkable(); no tiene sprite propio.
    // En el editor se renderiza como rectangulo rojo desde DrawObjects().
    class CollisionBlock : public CosmicEngine::Object
    {
    public:
        CollisionBlock(int objectId, Cell cell);
        void draw() const override {}
        Cell GetCell() const { return cell; }

    private:
        int objectId;
        Cell cell;
    };
}
