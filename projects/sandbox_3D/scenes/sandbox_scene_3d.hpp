#ifndef SANDBOX_SCENE_3D_HPP
#define SANDBOX_SCENE_3D_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/controllers/camera/classic_3d_camera_controller.hpp>

#include <memory>
#include <string>
#include <vector>

namespace CosmicEngine
{
    class Light;
}

class Sandbox3DScene : public CosmicEngine::Scene
{
private:
    std::string sceneFontKey;
    std::string cubeTextureKey;
    std::string volumeTextureKey;
    std::string hammerModelKey;
    std::string lightCubeTextureKey;
    glm::vec3 lightMarkerScale;
    std::size_t staticLightCount;
    std::size_t dynamicLightCount;
    std::unique_ptr<CosmicEngine::Classic3DCameraController> cameraController;

    void Setup3DCamera();
    void SpawnSceneObjects();
    void CreateStaticLightStressVolume();
    void CreateDynamicLightShowcase();
    void ToggleStressDisplayMode();
    std::string BuildOverlayText() const;

public:
    Sandbox3DScene();

    void init() override;
    void reset() override;
    void draw() override;
    void update(double deltaTime) override;
    void loadResources() override;
};

#endif