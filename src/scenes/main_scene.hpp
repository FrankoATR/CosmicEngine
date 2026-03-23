#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <CosmicEngine/collisions/CollisionArea.hpp>
#include <CosmicEngine/Models/Scene/scene.hpp>

#include <string>

namespace CosmicEngine
{
    class UIElement;
}

class JsonDemoObject;

class MainScene : public CosmicEngine::Scene
{
private:
    CosmicEngine::CollisionType currentCollisionType;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    std::string jsonSavePath;
    CosmicEngine::UIElement *demoButton;

    void SpawnCollisionTestObjects(int count);
    void ClearJsonDemoObjects();
    void CreateJsonDemoObject();
    void SaveJsonDemoObjects();
    void LoadJsonDemoObjects();
#endif

    void ConfigureCollisionTestArea(CosmicEngine::CollisionType type);

public:
    MainScene();

    void init() override;
    void reset() override;
    void draw() override;
    void update(double deltaTime) override;
    void loadResources() override;
};

#endif // MAINSCENE_HPP