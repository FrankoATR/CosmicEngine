#include "main_scene.hpp"

#include "../utilities/paths.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/models/animation/animation_clip.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    #include "../entities/json_demo_object.hpp"
    #include <CosmicEngine/managers/storage/json/json_manager.hpp>
    #include <CosmicEngine/models/ui/derived/ui_button.hpp>
    #include <random>
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    #include "../entities/collision_demo_object_3d.hpp"
    #include <CosmicEngine/models/ui/derived/ui_button.hpp>
    #include <CosmicEngine/models/ui/derived/ui_text.hpp>
    #include <random>
#endif

#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include CAMERAMANAGER_HEADER
#include EVENTMANAGER_HEADER
#include SCHEDULEMANAGER_HEADER
#include ANIMATIONMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include BODYMANAGER_HEADER
#include AUDIOMANAGER_HEADER
#include UIMANAGER_HEADER

#include <iostream>
#include <memory>

namespace
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    constexpr glm::vec2 kCollisionAreaPosition(-1000.0f, -1000.0f);
    constexpr glm::vec2 kCollisionAreaSize(1840.0f, 1420.0f);
    constexpr int kInitialTestObjectCount = 18;
    constexpr const char *kDemoSpriteSheetKey = "json_demo_sheet";
    constexpr int kDemoSpriteSheetRows = 4;
    constexpr int kDemoSpriteSheetColumns = 4;
    constexpr int kDemoSpriteSheetPadding = 0;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    constexpr glm::vec3 kCollisionAreaPosition(-48.0f, -24.0f, -118.0f);
    constexpr glm::vec3 kCollisionAreaSize(96.0f, 48.0f, 96.0f);
    constexpr int kInitialTestObjectCount3D = 180;

#else
    #error "[MainScene] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
MainScene::MainScene()
    : Scene("MainScene"),
      currentCollisionType(CosmicEngine::CollisionType::Grid),
      jsonSavePath("json_demo_save.json"),
            demoButton(nullptr),
            jsonSpawnEventListenerId(0),
            jsonSpawnScheduledTaskId(0),
            demo2DCameraController(nullptr)
{
}

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
MainScene::MainScene()
    : Scene("MainScene"),
      currentCollisionType(CosmicEngine::CollisionType::Grid),
      demo3DTitleText(nullptr),
      demo3DModeText(nullptr),
      demo3DControlsText(nullptr),
      demo3DClearButton(nullptr),
      demo3DMouseCaptureEnabled(true),
    demo3DKeepWorldCleared(false),
    demo3DCameraController(nullptr)
{
}

#else
    #error "[MainScene] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

void MainScene::loadResources()
{
}

void MainScene::init()
{
    RS_MN.LoadFont("test_font", "assets/fonts/ThaleahFat.ttf", 32);

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    Register2DAnimationExample();

    AUD_MN.Load("Music1", MUSIC_1_PATH, CosmicEngine::SoundType::Music);
    AUD_MN.SetPanRangeUnits(12.0f);
    AUD_MN.SetSfxVoicesPerSound(16);
    AUD_MN.SetListenerPosition(glm::vec3(0.0f));
    AUD_MN.Play("Music1", 0.1f, false);

    CAM_MN.SetFocusPosition(glm::vec2(0.0f));
    demo2DCameraController = std::make_unique<CosmicEngine::Classic2DCameraController>();

    RS_MN.LoadTexture("test_texture", "assets/textures/test.png");
    RS_MN.LoadTexture("test_texture2", "assets/textures/test.png");

    demoButton = new CosmicEngine::UIButton("Demo Button", "test_font", "test_texture2", {200.0f, 0.0f}, {150.0f, 50.0f}, false, true);
    UI_MN.AddElement(demoButton);

    ToogleShowBodys();
    JsonDemoObject::RegisterJsonSerialization();

    if (!JSON_MN.OpenFile(jsonSavePath))
    {
        std::cerr << "No se pudo abrir el archivo JSON: " << jsonSavePath << std::endl;
    }

    ConfigureCollisionTestArea(currentCollisionType);

    if (JSON_MN.HasClassSection(JsonDemoObject::StaticClassName()))
    {
        LoadJsonDemoObjects();
    }

    if (OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).empty())
    {
        SpawnCollisionTestObjects(kInitialTestObjectCount);
        SaveJsonDemoObjects();
    }

    Setup2DSpawnScheduler();

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    RS_MN.LoadTexture("test_texture2", "assets/textures/test.png");

    Setup3DCameraControls();
    this->Setup3DHUD();
    ConfigureCollisionTestArea(currentCollisionType);
    SpawnCollisionTestObjects3D(kInitialTestObjectCount3D);
    this->Update3DHUD();

#else
    #error "[MainScene] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

void MainScene::ConfigureCollisionTestArea(CosmicEngine::CollisionType type)
{
    currentCollisionType = type;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    BOD_MN.CreateCollisionArea(type, kCollisionAreaPosition, kCollisionAreaSize, 80, 6, 4);
    JsonDemoObject::SetMovementArea(kCollisionAreaPosition, kCollisionAreaSize);

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    BOD_MN.CreateCollisionArea(type, kCollisionAreaPosition, kCollisionAreaSize, 16, 5, 4);
    CollisionDemoObject3D::SetMovementArea(kCollisionAreaPosition, kCollisionAreaSize);

#else
    #error "[MainScene] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
void MainScene::Register2DAnimationExample()
{
    // Example configuration for spritesheet animation in the 2D demo.
    // Replace test.png with your own spritesheet and adjust rows/columns/padding.
    RS_MN.LoadTextureSheet(kDemoSpriteSheetKey, "assets/textures/player2.png", kDemoSpriteSheetRows, kDemoSpriteSheetColumns, kDemoSpriteSheetPadding);

    auto registerDirectionalClip = [](const std::string &clipName, int row)
    {
        if (ANM_MN.HasClip(clipName))
        {
            return;
        }

        std::vector<CosmicEngine::AnimationFrame> frames = {
            {row, 0, 0.14},
            {row, 1, 0.14},
            {row, 2, 0.14},
            {row, 3, 0.14}
        };

        ANM_MN.RegisterClip(CosmicEngine::AnimationClip(clipName, kDemoSpriteSheetKey, frames, true));
    };

    registerDirectionalClip("json_demo_move_down", 0);
    registerDirectionalClip("json_demo_move_up", 1);
    registerDirectionalClip("json_demo_move_left", 2);
    registerDirectionalClip("json_demo_move_right", 3);
}

void MainScene::Setup2DSpawnScheduler()
{
    static const std::string kSpawnEventName = "main_scene.spawn_random_json_object";

    if (jsonSpawnEventListenerId == 0)
    {
        jsonSpawnEventListenerId = EVT_MN.RegisterEventListener<const nlohmann::json &>(
            kSpawnEventName,
            std::function<void(const nlohmann::json &)>([this](const nlohmann::json &payload)
            {
                SpawnJsonDemoObjectFromPayload(payload);
            }));
    }

    if (jsonSpawnScheduledTaskId != 0 && SCH_MN.HasTask(jsonSpawnScheduledTaskId))
    {
        return;
    }

    nlohmann::json payload = {
        {"randomPosition", true},
        {"size", {64.0f, 64.0f}},
        {"health", 100},
        {"layerId", 1},
        {"labelPrefix", "JSON Auto"},
        {"color", {0.35f, 0.85f, 0.55f}}
    };

    jsonSpawnScheduledTaskId = SCH_MN.ScheduleInterval(2.0, kSpawnEventName, payload, -1, "spawn_random_json_object_every_2_seconds");
}

void MainScene::ClearJsonDemoObjects()
{
    auto existingObjects = OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName());
    for (auto *object : existingObjects)
    {
        if (object)
        {
            OBJ_MN.Remove(object->GetID());
        }
    }
}

void MainScene::SpawnRandomJsonDemoObject()
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> xDistribution(kCollisionAreaPosition.x, kCollisionAreaPosition.x + kCollisionAreaSize.x - 64.0f);
    std::uniform_real_distribution<float> yDistribution(kCollisionAreaPosition.y, kCollisionAreaPosition.y + kCollisionAreaSize.y - 64.0f);
    std::uniform_real_distribution<float> colorDistribution(0.25f, 0.95f);

    std::size_t objectCount = OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).size();

    JsonDemoObject *newObject = new JsonDemoObject(
        {xDistribution(generator), yDistribution(generator)},
        {64.0f, 64.0f},
        "JSON Auto " + std::to_string(objectCount + 1),
        100,
        1);
    newObject->SetColor({colorDistribution(generator), colorDistribution(generator), colorDistribution(generator)});
    OBJ_MN.Add(newObject);
}

void MainScene::SpawnJsonDemoObjectFromPayload(const nlohmann::json &payload)
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> xDistribution(kCollisionAreaPosition.x, kCollisionAreaPosition.x + kCollisionAreaSize.x - 64.0f);
    std::uniform_real_distribution<float> yDistribution(kCollisionAreaPosition.y, kCollisionAreaPosition.y + kCollisionAreaSize.y - 64.0f);

    bool randomPosition = payload.value("randomPosition", true);

    glm::vec2 size(64.0f, 64.0f);
    if (payload.contains("size") && payload["size"].is_array() && payload["size"].size() >= 2)
    {
        size.x = payload["size"][0].get<float>();
        size.y = payload["size"][1].get<float>();
    }

    std::uniform_real_distribution<float> sizedXDistribution(kCollisionAreaPosition.x, kCollisionAreaPosition.x + kCollisionAreaSize.x - size.x);
    std::uniform_real_distribution<float> sizedYDistribution(kCollisionAreaPosition.y, kCollisionAreaPosition.y + kCollisionAreaSize.y - size.y);

    glm::vec2 position(
        randomPosition ? sizedXDistribution(generator) : payload.value("x", kCollisionAreaPosition.x),
        randomPosition ? sizedYDistribution(generator) : payload.value("y", kCollisionAreaPosition.y));

    glm::vec3 color(0.35f, 0.85f, 0.55f);
    if (payload.contains("color") && payload["color"].is_array() && payload["color"].size() >= 3)
    {
        color.r = payload["color"][0].get<float>();
        color.g = payload["color"][1].get<float>();
        color.b = payload["color"][2].get<float>();
    }

    int health = payload.value("health", 100);
    short int layerId = static_cast<short int>(payload.value("layerId", 1));
    std::string labelPrefix = payload.value("labelPrefix", std::string("JSON Auto"));
    std::size_t objectCount = OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).size();

    JsonDemoObject *newObject = new JsonDemoObject(
        position,
        size,
        labelPrefix + " " + std::to_string(objectCount + 1),
        health,
        layerId);
    newObject->SetColor(color);
    OBJ_MN.Add(newObject);
}

void MainScene::SpawnCollisionTestObjects(int count)
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> xDistribution(kCollisionAreaPosition.x, kCollisionAreaPosition.x + kCollisionAreaSize.x - 64.0f);
    std::uniform_real_distribution<float> yDistribution(kCollisionAreaPosition.y, kCollisionAreaPosition.y + kCollisionAreaSize.y - 64.0f);
    std::uniform_real_distribution<float> colorDistribution(0.25f, 0.95f);

    ClearJsonDemoObjects();

    for (int index = 0; index < count; ++index)
    {
        JsonDemoObject *newObject = new JsonDemoObject(
            {xDistribution(generator), yDistribution(generator)},
            {64.0f, 64.0f},
            "JSON " + std::to_string(index + 1),
            100,
            1);
        newObject->SetColor({colorDistribution(generator), colorDistribution(generator), colorDistribution(generator)});
        OBJ_MN.Add(newObject);
    }
}

void MainScene::CreateJsonDemoObject()
{
    JsonDemoObject *createdObject = new JsonDemoObject({-220.0f, -40.0f}, {64.0f, 64.0f}, "Objeto JSON", 100, 1);
    createdObject->SetColor({0.2f, 0.9f, 0.4f});
    OBJ_MN.Add(createdObject);
}

void MainScene::SaveJsonDemoObjects()
{
    JSON_MN.SaveObjectsData(JsonDemoObject::StaticClassName());

    if (JSON_MN.SaveFile(true))
    {
        std::cout << "Datos guardados en " << jsonSavePath << std::endl;
    }
    else
    {
        std::cerr << "No se pudieron guardar los datos JSON" << std::endl;
    }
}

void MainScene::LoadJsonDemoObjects()
{
    ClearJsonDemoObjects();
    JSON_MN.LoadObjectsData(JsonDemoObject::StaticClassName());

    if (!OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).empty())
    {
        std::cout << "Datos cargados desde " << jsonSavePath << std::endl;
    }
    else
    {
        std::cout << "No había objetos guardados para cargar" << std::endl;
    }
}

void MainScene::Setup3DHUD()
{
}

void MainScene::Update3DHUD()
{
}
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
void MainScene::ClearCollisionDemoObjects3D()
{
    auto existingObjects = OBJ_MN.FindByClassName(CollisionDemoObject3D::StaticClassName());
    for (auto *object : existingObjects)
    {
        if (object)
        {
            OBJ_MN.Remove(object->GetID());
        }
    }
}

void MainScene::SpawnCollisionTestObjects3D(int count)
{
    static std::mt19937 generator(std::random_device{}());
    constexpr glm::vec3 objectSize(2.0f, 2.0f, 2.0f);

    std::uniform_real_distribution<float> xDistribution(kCollisionAreaPosition.x + 2.0f, kCollisionAreaPosition.x + kCollisionAreaSize.x - objectSize.x - 2.0f);
    std::uniform_real_distribution<float> yDistribution(kCollisionAreaPosition.y + 2.0f, kCollisionAreaPosition.y + kCollisionAreaSize.y - objectSize.y - 2.0f);
    std::uniform_real_distribution<float> zDistribution(kCollisionAreaPosition.z + 2.0f, kCollisionAreaPosition.z + kCollisionAreaSize.z - objectSize.z - 2.0f);
    std::uniform_real_distribution<float> colorDistribution(0.25f, 0.95f);
    std::uniform_real_distribution<float> speedDistribution(14.0f, 24.0f);

    demo3DKeepWorldCleared = false;
    ClearCollisionDemoObjects3D();

    for (int index = 0; index < count; ++index)
    {
        CollisionDemoObject3D *newObject = new CollisionDemoObject3D(
            {xDistribution(generator), yDistribution(generator), zDistribution(generator)},
            objectSize,
            "Cube " + std::to_string(index + 1),
            speedDistribution(generator));
        newObject->SetColor({colorDistribution(generator), colorDistribution(generator), colorDistribution(generator)});
        OBJ_MN.Add(newObject);
    }
}

void MainScene::Setup3DCameraControls()
{
    CAM_MN.SetMovementSpeed(28.0f);
    if (!demo3DCameraController)
    {
        demo3DCameraController = std::make_unique<CosmicEngine::Classic3DCameraController>();
        demo3DCameraController->Attach(GM_MN);
    }

    Set3DMouseCaptureEnabled(true);
}

void MainScene::Set3DMouseCaptureEnabled(bool enabled)
{
    demo3DMouseCaptureEnabled = enabled;

    if (demo3DCameraController)
    {
        demo3DCameraController->SetMouseCaptureEnabled(enabled);
    }

    if (demo3DClearButton)
    {
        demo3DClearButton->SetVisible(!enabled);
    }

    Update3DHUD();
}

void MainScene::Setup3DHUD()
{
    demo3DTitleText = new CosmicEngine::UIText(
        "Demo colisiones 3D | 1 Grid | 2 Octree | R Respawn",
        "test_font",
        glm::vec2(0.0f, 24.0f),
        glm::vec2(1920.0f, 36.0f),
        true);
    demo3DTitleText->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
    UI_MN.AddElement(demo3DTitleText);

    demo3DModeText = new CosmicEngine::UIText(
        "",
        "test_font",
        glm::vec2(0.0f, 62.0f),
        glm::vec2(1920.0f, 36.0f),
        true);
    demo3DModeText->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
    UI_MN.AddElement(demo3DModeText);

    demo3DControlsText = new CosmicEngine::UIText(
        "",
        "test_font",
        glm::vec2(0.0f, 100.0f),
        glm::vec2(1920.0f, 36.0f),
        true);
    demo3DControlsText->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
    UI_MN.AddElement(demo3DControlsText);

    demo3DClearButton = new CosmicEngine::UIButton(
        "Limpiar mundo",
        "test_font",
        "test_texture2",
        glm::vec2(820.0f, 170.0f),
        glm::vec2(280.0f, 72.0f),
        true,
        false);
    demo3DClearButton->SetOnClick([this]()
    {
        demo3DKeepWorldCleared = true;
        ClearCollisionDemoObjects3D();
        Update3DHUD();
    });
    UI_MN.AddElement(demo3DClearButton);
}

void MainScene::Update3DHUD()
{
    if (!demo3DModeText)
    {
        return;
    }

    std::string collisionMode = currentCollisionType == CosmicEngine::CollisionType::Grid ? "Grid" : "Octree";
    demo3DModeText->SetText("Modo actual: " + collisionMode + " | WASD mover | Space subir | Shift bajar | Mouse mirar");

    if (demo3DControlsText)
    {
        if (demo3DMouseCaptureEnabled)
        {
            demo3DControlsText->SetText("Rueda zoom | B cuerpos debug | ESC liberar mouse");
        }
        else
        {
            demo3DControlsText->SetText("ESC volver a camara | Click en el boton para limpiar todos los objetos");
        }
    }
}

void MainScene::Update3DCamera(double deltaTime)
{
    if (demo3DCameraController)
    {
        demo3DCameraController->Update(deltaTime);
    }
}
#endif

void MainScene::reset()
{
}

void MainScene::draw()
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    std::string collisionMode = currentCollisionType == CosmicEngine::CollisionType::Grid ? "Grid" : "QuadTree";
    RS_MN.RenderText("Prueba colisiones JSON | 1 Grid | 2 QuadTree | R Respawn | G Guarda | C Carga", "test_font", {-420.0f, -210.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
    RS_MN.RenderText("Modo actual: " + collisionMode + " | WASD mueve camara | Click derecho crea objeto | Animacion: sheet 2x2", "test_font", {-420.0f, -180.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
    RS_MN.RenderRectangle(kCollisionAreaPosition, kCollisionAreaPosition + kCollisionAreaSize, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.0f, 0.6f, 1.0f), 0.5f, 2.5f, false, CosmicEngine::ViewType::Ortho);

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    RS_MN.RenderParallelepipedLines(kCollisionAreaPosition, kCollisionAreaSize, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.65f, 1.0f), 0.85f, 2.25f, false, CosmicEngine::ViewType::Projection);

#else
    #error "[MainScene] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

void MainScene::update(double deltaTime)
{
    (void)deltaTime;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
    {
        GM_MN.endprogram();
    }
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
    {
        Set3DMouseCaptureEnabled(!demo3DMouseCaptureEnabled);
    }
#endif

    if (INP_MN.IsKeyPressed(GLFW_KEY_1, CosmicEngine::KeyDown))
    {
        ConfigureCollisionTestArea(CosmicEngine::CollisionType::Grid);

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        SpawnCollisionTestObjects(kInitialTestObjectCount);
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        SpawnCollisionTestObjects3D(kInitialTestObjectCount3D);
#endif

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    this->Update3DHUD();
#endif
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_2, CosmicEngine::KeyDown))
    {
        ConfigureCollisionTestArea(CosmicEngine::CollisionType::QuadTree);

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        SpawnCollisionTestObjects(kInitialTestObjectCount);
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        SpawnCollisionTestObjects3D(kInitialTestObjectCount3D);
#endif

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    this->Update3DHUD();
#endif
    }

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    if (INP_MN.IsKeyPressed(GLFW_KEY_ENTER, CosmicEngine::KeyDown))
    {
        AUD_MN.SetPosition("Music1", 10000);
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_G, CosmicEngine::KeyDown))
    {
        SaveJsonDemoObjects();
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_C, CosmicEngine::KeyDown))
    {
        LoadJsonDemoObjects();
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_R, CosmicEngine::KeyDown))
    {
        SpawnCollisionTestObjects(kInitialTestObjectCount);
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_B, CosmicEngine::KeyDown))
    {
        ToogleShowBodys();
    }

    if (demoButton && INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, CosmicEngine::KeyDown) && demoButton->MouseHover())
    {
        ClearJsonDemoObjects();
    }

    if (INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, CosmicEngine::KeyDown) && !UI_MN.IsMouseHoverAny())
    {
        glm::vec2 mousePos = INP_MN.GetMousePosition();

        for (auto *object : OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()))
        {
            if (!object)
            {
                continue;
            }

            glm::vec2 objectPosition = object->GetPosition();
            glm::vec2 objectSize = object->GetSize();
            if (mousePos.x >= objectPosition.x && mousePos.x <= objectPosition.x + objectSize.x &&
                mousePos.y >= objectPosition.y && mousePos.y <= objectPosition.y + objectSize.y)
            {
                OBJ_MN.Remove(object->GetID());
                break;
            }
        }
    }

    if (INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT, CosmicEngine::KeyDown) && !UI_MN.IsMouseHoverAny())
    {
        glm::vec2 mousePos = INP_MN.GetMousePosition();
        JsonDemoObject *newObject = new JsonDemoObject(mousePos - glm::vec2(8.0f, 8.0f), {16.0f, 16.0f}, "test new", 100, 1);
        newObject->SetColor({0.9f, 0.3f, 0.5f});
        OBJ_MN.Add(newObject);
    }

    if (demo2DCameraController)
    {
        demo2DCameraController->Update(deltaTime);
    }
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    if (!BOD_MN.HasCollisionArea())
    {
        ConfigureCollisionTestArea(currentCollisionType);
        this->Update3DHUD();
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_B, CosmicEngine::KeyDown))
    {
        ToogleShowBodys();
    }

    if (demo3DMouseCaptureEnabled)
    {
        Update3DCamera(deltaTime);
    }

    if (!demo3DKeepWorldCleared && OBJ_MN.FindByClassName(CollisionDemoObject3D::StaticClassName()).empty())
    {
        SpawnCollisionTestObjects3D(kInitialTestObjectCount3D);
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_R, CosmicEngine::KeyDown))
    {
        SpawnCollisionTestObjects3D(kInitialTestObjectCount3D);
    }
#endif

    if (INP_MN.IsKeyPressed(GLFW_KEY_F12, CosmicEngine::KeyDown))
    {
        GM_MN.enableVsync();
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_F11, CosmicEngine::KeyDown))
    {
        GM_MN.toggleFullscreen();
    }
}
