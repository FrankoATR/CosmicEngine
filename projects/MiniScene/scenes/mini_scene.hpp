#pragma once
#include <CosmicEngine/models/scene/scene.hpp>

namespace MiniScene
{
    class MiniScene : public CosmicEngine::Scene
    {
    public:
        MiniScene() : CosmicEngine::Scene("MiniScene") {}
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
    };
}
