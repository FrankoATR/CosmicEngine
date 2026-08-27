#pragma once
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/body/body.hpp>

namespace MiniScene
{
    class PlayerObject : public CosmicEngine::Object
    {
    public:
        explicit PlayerObject(glm::vec2 position);
        void init() override;
        void update(float deltaTime) override;
        void draw() const override;

    private:
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);
        bool touching = false;
        bool wasTouching = false;
    };
}
