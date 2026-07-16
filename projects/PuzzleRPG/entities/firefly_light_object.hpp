#ifndef PUZZLERPG_FIREFLY_LIGHT_OBJECT_HPP
#define PUZZLERPG_FIREFLY_LIGHT_OBJECT_HPP

#include "../systems/grid.hpp"

#include <CosmicEngine/models/light/light.hpp>
#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    class FireflyLightObject : public CosmicEngine::Object
    {
    public:
        FireflyLightObject(int id, Cell anchorCell, float seed);
        ~FireflyLightObject() override;

        void update(float deltaTime) override;
        void draw() const override;

    private:
        int objectId;
        Cell anchorCell;
        CosmicEngine::Light *light = nullptr;
        float animationTime = 0.0f;
        float seed = 0.0f;
        float flicker = 1.0f;

        glm::vec2 anchorPosition;
        glm::vec2 currentCenter;

        void UpdateMotion();
        void UpdateLight();
        void ReleaseLight();
    };
}

#endif
