#include "sandbox_scene_3d.hpp"

#include "../entities/sandbox_hammer_object_3d.hpp"
#include "../entities/sandbox_light_object_3d.hpp"
#include "../entities/sandbox_rotating_cube_object_3d.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#include INPUTMANAGER_HEADER
#include CAMERAMANAGER_HEADER
#include LIGHTMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include SCENEMANAGER_HEADER

#include <random>
#include <sstream>

namespace
{
    constexpr int kSandbox3DStaticLightCountX = 5;
    constexpr int kSandbox3DStaticLightCountY = 5;
    constexpr int kSandbox3DStaticLightCountZ = 4;
    constexpr int kSandbox3DDynamicLightCount = 6;
    constexpr float kSandbox3DStressVolumeSide = 200.0f;
    constexpr float kSandbox3DLightHalfExtent = kSandbox3DStressVolumeSide * 0.5f;

    glm::vec3 GenerateRandomPositionInsideVolume(const glm::vec3 &center, const glm::vec3 &size, const glm::vec3 &padding)
    {
        static std::mt19937 generator(std::random_device{}());

        const glm::vec3 halfSize = size * 0.5f - padding;
        std::uniform_real_distribution<float> xDistribution(center.x - halfSize.x, center.x + halfSize.x);
        std::uniform_real_distribution<float> yDistribution(center.y - halfSize.y, center.y + halfSize.y);
        std::uniform_real_distribution<float> zDistribution(center.z - halfSize.z, center.z + halfSize.z);

        return glm::vec3(
            xDistribution(generator),
            yDistribution(generator),
            zDistribution(generator));
    }
}

Sandbox3DScene::Sandbox3DScene()
    : Scene("Sandbox3DScene"),
    sceneFontKey("sandbox_scene_font"),
    cubeTextureKey("sandbox_scene_cube_test_texture"),
    volumeTextureKey("sandbox_scene_volume_test_texture"),
    hammerModelKey("sandbox_scene_hammer_model"),
    lightCubeTextureKey("sandbox_scene_cube_light_texture"),
    lightMarkerScale(0.7f, 0.7f, 0.7f),
    staticLightCount(0),
    dynamicLightCount(0),
    cameraController(nullptr)
{
}

void Sandbox3DScene::loadResources()
{
    RS_MN.LoadFont(sceneFontKey, "assets/fonts/ThaleahFat.ttf", 32);
    RS_MN.LoadTexture(cubeTextureKey, "assets/textures/cube_test.png");
    RS_MN.LoadTexture(volumeTextureKey, "assets/textures/test.png");
    RS_MN.LoadTexture(lightCubeTextureKey, "assets/textures/cube_light.png");
    RS_MN.Load3DModel(hammerModelKey, "assets/models/warhammer.glb");
}

void Sandbox3DScene::init()
{
    loadResources();

    INP_MN.RegisterAction("sandbox_toggle_stress_display", {{GLFW_KEY_F12}, {GLFW_GAMEPAD_BUTTON_Y}, {}, {}});

    SCN_MN.SetBackgroundColor(glm::vec3(0.01f, 0.01f, 0.02f));
    Setup3DCamera();
    cameraController = std::make_unique<CosmicEngine::Classic3DCameraController>();
    cameraController->Attach(GM_MN);
    cameraController->SetMouseCaptureEnabled(true);

    LGT_MN.SetGlobalAmbientLightDirection(glm::vec3(-0.15f, -1.0f, -0.12f));
    LGT_MN.SetGlobalAmbientLightColor(glm::vec3(0.025f, 0.025f, 0.030f));
    LGT_MN.SetGlobalAmbientLightDiffuse(glm::vec3(0.06f, 0.06f, 0.08f));
    LGT_MN.SetGlobalAmbientLightSpecular(glm::vec3(0.02f, 0.02f, 0.03f));
    LGT_MN.SetMaxActivePointLights(8);

    SpawnSceneObjects();
}

void Sandbox3DScene::reset()
{
}

void Sandbox3DScene::draw()
{
    RS_MN.RenderParallelepipedTextureLit(
        volumeTextureKey,
        glm::vec3(0.0f),
        glm::vec3(kSandbox3DStressVolumeSide),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.18f, 0.18f, 0.18f),
        1.0f,
        CosmicEngine::ViewType::Projection);

    RS_MN.RenderParallelepipedLines(
        glm::vec3(0.0f),
        glm::vec3(kSandbox3DStressVolumeSide),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.18f, 0.85f, 1.0f),
        0.75f,
        1.8f,
        false,
        CosmicEngine::ViewType::Projection);

    RS_MN.RenderText(
        BuildOverlayText(),
        sceneFontKey,
        {35.0f, 35.0f, 0.0f},
        {0.55f, 0.55f, 1.0f},
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(1.0f),
        1.0f,
        CosmicEngine::ViewType::UI);
}

void Sandbox3DScene::update(double deltaTime)
{
    if (INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
    {
        GM_MN.endprogram();
    }

    if (INP_MN.IsActionPressed("sandbox_toggle_stress_display", CosmicEngine::KeyDown))
    {
        ToggleStressDisplayMode();
    }

    if (cameraController)
    {
        cameraController->Update(deltaTime);
    }
}

void Sandbox3DScene::Setup3DCamera()
{
    CAM_MN.SetZoom(45.0f);
    CAM_MN.SetMovementSpeed(36.0f);
    CAM_MN.SetFocusPosition(glm::vec3(0.0f, 16.0f, 120.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void Sandbox3DScene::SpawnSceneObjects()
{
    OBJ_MN.Add(new SandboxRotatingCubeObject3D(
        cubeTextureKey,
        glm::vec3(0.0f),
        glm::vec3(14.0f),
        glm::vec3(0.0f, 32.0f, 0.0f),
        glm::vec3(0.0f, 8.0f, 0.0f),
        glm::vec3(1.0f, 0.45f, 0.15f)));

    auto *hammerA = new SandboxHammerObject3D(
        hammerModelKey,
        glm::vec3(-26.0f, 18.0f, -12.0f),
        glm::vec3(16.0f),
        glm::vec3(0.0f, 135.0f, 18.0f),
        glm::vec3(0.0f, 32.0f, 12.0f));
    hammerA->SetColor(glm::vec3(1.0f, 0.92f, 0.78f));
    OBJ_MN.Add(hammerA);

    auto *hammerB = new SandboxHammerObject3D(
        hammerModelKey,
        glm::vec3(28.0f, -14.0f, 18.0f),
        glm::vec3(14.0f),
        glm::vec3(14.0f, 220.0f, 0.0f),
        glm::vec3(6.0f, -26.0f, 9.0f));
    hammerB->SetColor(glm::vec3(0.78f, 0.96f, 1.0f));
    OBJ_MN.Add(hammerB);

    CreateStaticLightStressVolume();
    CreateDynamicLightShowcase();
}

void Sandbox3DScene::CreateStaticLightStressVolume()
{
    staticLightCount = 0;

    const float xSpacing = kSandbox3DStressVolumeSide / static_cast<float>(kSandbox3DStaticLightCountX);
    const float ySpacing = kSandbox3DStressVolumeSide / static_cast<float>(kSandbox3DStaticLightCountY);
    const float zSpacing = kSandbox3DStressVolumeSide / static_cast<float>(kSandbox3DStaticLightCountZ);
    const glm::vec3 start(
        -kSandbox3DLightHalfExtent + xSpacing * 0.5f,
        -kSandbox3DLightHalfExtent + ySpacing * 0.5f,
        -kSandbox3DLightHalfExtent + zSpacing * 0.5f);

    for (int x = 0; x < kSandbox3DStaticLightCountX; ++x)
    {
        for (int y = 0; y < kSandbox3DStaticLightCountY; ++y)
        {
            for (int z = 0; z < kSandbox3DStaticLightCountZ; ++z)
            {
                const glm::vec3 position(
                    start.x + static_cast<float>(x) * xSpacing,
                    start.y + static_cast<float>(y) * ySpacing,
                    start.z + static_cast<float>(z) * zSpacing);

                const glm::vec3 colorMix(
                    0.25f + 0.70f * static_cast<float>(x) / static_cast<float>(kSandbox3DStaticLightCountX - 1),
                    0.25f + 0.70f * static_cast<float>(y) / static_cast<float>(kSandbox3DStaticLightCountY - 1),
                    0.25f + 0.70f * static_cast<float>(z) / static_cast<float>(kSandbox3DStaticLightCountZ - 1));

                OBJ_MN.Add(new SandboxLightObject3D(
                    lightCubeTextureKey,
                    position,
                    lightMarkerScale,
                    colorMix * 0.018f,
                    colorMix * 0.75f,
                    colorMix * 0.42f,
                    CosmicEngine::LightMobility::Static,
                    glm::vec3(0.0f),
                    glm::vec3(kSandbox3DStressVolumeSide),
                    0.0f,
                    glm::vec3(0.0f)));
                ++staticLightCount;
            }
        }
    }
}

void Sandbox3DScene::CreateDynamicLightShowcase()
{
    dynamicLightCount = 0;
    const glm::vec3 movementAreaCenter(0.0f);
    const glm::vec3 movementAreaSize(kSandbox3DStressVolumeSide * 0.78f);
    const glm::vec3 padding = lightMarkerScale * 0.75f;

    for (int index = 0; index < kSandbox3DDynamicLightCount; ++index)
    {
        const glm::vec3 position = GenerateRandomPositionInsideVolume(movementAreaCenter, movementAreaSize, padding);
        const glm::vec3 colorMix(
            0.35f + 0.10f * static_cast<float>(index % 3),
            0.35f + 0.18f * static_cast<float>((index + 1) % 3),
            0.55f + 0.07f * static_cast<float>(index));

        OBJ_MN.Add(new SandboxLightObject3D(
            lightCubeTextureKey,
            position,
            lightMarkerScale * 1.5f,
            colorMix * 0.08f,
            colorMix * 2.35f,
            colorMix * 1.45f,
            CosmicEngine::LightMobility::Dynamic,
            movementAreaCenter,
            movementAreaSize,
            14.0f + static_cast<float>(index),
            glm::vec3(12.0f, 22.0f, 8.0f) + glm::vec3(static_cast<float>(index))));
        ++dynamicLightCount;
    }
}

void Sandbox3DScene::ToggleStressDisplayMode()
{
    if (GM_MN.isFullScreen())
    {
        GM_MN.toggleFullscreen();
        GM_MN.disableVsync();
        return;
    }

    GM_MN.enableVsync();
    GM_MN.toggleFullscreen();
}

std::string Sandbox3DScene::BuildOverlayText() const
{
    std::ostringstream stream;
    stream
        << "Sandbox 3D Stress | luces estaticas: " << staticLightCount
        << " | luces dinamicas: " << dynamicLightCount
        << " | FPS: " << GM_MN.getCurrentFps()
        << " | VSync: " << (GM_MN.isVsyncEnabled() ? "ON" : "OFF")
        << " | Fullscreen: " << (GM_MN.isFullScreen() ? "ON" : "OFF")
        << " | F12 alterna pantalla completa + vsync";
    return stream.str();
}