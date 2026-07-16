#ifndef PUZZLERPG_TORCH_LIGHT_OBJECT_HPP
#define PUZZLERPG_TORCH_LIGHT_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/light/light.hpp>
#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    class TorchLightObject : public CosmicEngine::Object
    {
    public:
        TorchLightObject(int id, Cell cell);
        ~TorchLightObject() override;

        void update(float deltaTime) override;
        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell() const { return cell; }

    private:
        int objectId;
        Cell cell;
        CosmicEngine::Light *light = nullptr;
        float animationTime = 0.0f;
        float flicker = 1.0f;
        SpriteSheetVisual spriteVisual;

        glm::vec2 FlameCenter() const;
        void UpdateLight();
        void ReleaseLight();
    };
}

#endif
