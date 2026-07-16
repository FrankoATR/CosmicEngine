#ifndef PUZZLERPG_SIGN_OBJECT_HPP
#define PUZZLERPG_SIGN_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    class SignObject : public CosmicEngine::Object
    {
    public:
        SignObject(int id, Cell cell);
        ~SignObject() override = default;

        void update(float deltaTime) override;
        void draw() const override;

        int GetObjectId() const { return objectId; }
        Cell GetCell() const { return cell; }

    private:
        int objectId;
        Cell cell;
        SpriteSheetVisual spriteVisual;
    };
}

#endif