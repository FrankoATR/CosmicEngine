#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <CosmicEngine/collisions/CollisionArea.hpp>
#include <CosmicEngine/controllers/camera/camera_controller.hpp>
#include <CosmicEngine/Models/Scene/scene.hpp>

#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    #include <CosmicEngine/controllers/camera/classic_2d_camera_controller.hpp>
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    #include <CosmicEngine/controllers/camera/classic_3d_camera_controller.hpp>
#endif

namespace CosmicEngine
{
    class UIElement;
    class UIText;
    class UIButton;
}

class JsonDemoObject;

class MainScene : public CosmicEngine::Scene
{
private:
    CosmicEngine::CollisionType currentCollisionType;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    std::string jsonSavePath;
    CosmicEngine::UIElement *demoButton;
    std::uint64_t jsonSpawnEventListenerId;
    std::uint64_t jsonSpawnScheduledTaskId;
    std::unique_ptr<CosmicEngine::ICameraController> demo2DCameraController;

    void Register2DAnimationExample();
    void Setup2DSpawnScheduler();
    void SpawnCollisionTestObjects(int count);
    void SpawnRandomJsonDemoObject();
    void SpawnJsonDemoObjectFromPayload(const nlohmann::json &payload);
    void ClearJsonDemoObjects();
    void CreateJsonDemoObject();
    void SaveJsonDemoObjects();
    void LoadJsonDemoObjects();
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    CosmicEngine::UIText *demo3DTitleText;
    CosmicEngine::UIText *demo3DModeText;
    CosmicEngine::UIText *demo3DControlsText;
    CosmicEngine::UIButton *demo3DClearButton;
    bool demo3DMouseCaptureEnabled;
    bool demo3DKeepWorldCleared;
    std::unique_ptr<CosmicEngine::Classic3DCameraController> demo3DCameraController;

    void SpawnCollisionTestObjects3D(int count);
    void ClearCollisionDemoObjects3D();
    void Setup3DCameraControls();
    void Set3DMouseCaptureEnabled(bool enabled);
    void Update3DCamera(double deltaTime);
#endif

    void Setup3DHUD();
    void Update3DHUD();

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