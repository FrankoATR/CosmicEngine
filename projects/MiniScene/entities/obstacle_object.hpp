#pragma once
#include <CosmicEngine/models/object/object.hpp>

namespace MiniScene
{
    class ObstacleObject : public CosmicEngine::Object
    {
    public:
        explicit ObstacleObject(glm::vec2 position);
        void init() override;
        void update(float) override {}
        void draw() const override;
    };
}
