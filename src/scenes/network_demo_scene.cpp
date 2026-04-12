#include "network_demo_scene.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/collisions/CollisionArea.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/managers/network/network_manager.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_field.hpp>

#include "../entities/network_demo_player.hpp"
#include "../entities/network_demo_projectile.hpp"

#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include CAMERAMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include BODYMANAGER_HEADER
#include UIMANAGER_HEADER

#include <iostream>
#include <random>
#include <glm/gtc/matrix_inverse.hpp>

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

namespace
{
    constexpr glm::vec3 kArenaPosition(-30.0f, -2.0f, -30.0f);
    constexpr glm::vec3 kArenaSize(60.0f, 20.0f, 60.0f);
    constexpr float kGroundY = kArenaPosition.y;

    std::mt19937 &GetRNG()
    {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }

    glm::vec3 RandomSpawnPosition()
    {
        std::uniform_real_distribution<float> xDist(kArenaPosition.x + 4.0f, kArenaPosition.x + kArenaSize.x - 4.0f);
        std::uniform_real_distribution<float> zDist(kArenaPosition.z + 4.0f, kArenaPosition.z + kArenaSize.z - 4.0f);
        return glm::vec3(xDist(GetRNG()), kGroundY, zDist(GetRNG()));
    }

    int RandomDamage()
    {
        std::uniform_int_distribution<int> dmgDist(1, 3);
        return dmgDist(GetRNG());
    }
}

// ══════════════════════════════════════════════════
// Constructor
// ══════════════════════════════════════════════════

NetworkDemoScene::NetworkDemoScene()
    : Scene("NetworkDemoScene"),
      currentState(GameState::Menu),
      nextProjectileId(0),
      stateUpdateTimer(0.0f),
      spawnCooldown(0.0f),
    pendingSpawnTimer(0.0f),
    hasPendingSpawn(false),
    hasPreviewSpawn(false),
    pendingSpawnPosition(0.0f),
    previewSpawnPosition(0.0f),
      btnServer(nullptr), btnClient(nullptr),
      txtTitle(nullptr), txtInstructions(nullptr),
      txtFieldIp(nullptr), txtFieldPort(nullptr),
      btnConnect(nullptr), btnBack(nullptr), txtConnectionStatus(nullptr),
      txtHudHealth(nullptr), txtHudInfo(nullptr), txtHudControls(nullptr),
      txtGameOver(nullptr), btnReturnMenu(nullptr),
      mouseCaptured(false)
{
}

// ══════════════════════════════════════════════════
// Scene lifecycle
// ══════════════════════════════════════════════════

void NetworkDemoScene::loadResources()
{
}

void NetworkDemoScene::init()
{
    RS_MN.LoadFont("demo_font", "assets/fonts/ThaleahFat.ttf", 32);
    RS_MN.LoadFont("demo_font_large", "assets/fonts/ThaleahFat.ttf", 48);

    BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid, kArenaPosition, kArenaSize, 10, 5, 4);
    NetworkDemoPlayer::SetMovementArea(kArenaPosition, kArenaSize);

    SetupMenuUI();
    SetupClientConnectionUI();
    SetupInGameHUD();
    SetupGameOverUI();

    SwitchToMenu();
}

void NetworkDemoScene::reset()
{
    CleanupGame();
}

void NetworkDemoScene::draw()
{
    if (currentState == GameState::Playing)
    {
        // Draw floor grid
        for (float x = kArenaPosition.x; x <= kArenaPosition.x + kArenaSize.x; x += 5.0f)
        {
            RS_MN.RenderLine(
                glm::vec3(x, kArenaPosition.y, kArenaPosition.z),
                glm::vec3(x, kArenaPosition.y, kArenaPosition.z + kArenaSize.z),
                glm::vec3(0.0f), glm::vec3(0.0f),
                glm::vec3(0.15f, 0.3f, 0.5f),
                0.3f, 1.0f,
                CosmicEngine::ViewType::Projection);
        }
        for (float z = kArenaPosition.z; z <= kArenaPosition.z + kArenaSize.z; z += 5.0f)
        {
            RS_MN.RenderLine(
                glm::vec3(kArenaPosition.x, kArenaPosition.y, z),
                glm::vec3(kArenaPosition.x + kArenaSize.x, kArenaPosition.y, z),
                glm::vec3(0.0f), glm::vec3(0.0f),
                glm::vec3(0.15f, 0.3f, 0.5f),
                0.3f, 1.0f,
                CosmicEngine::ViewType::Projection);
        }

        RS_MN.RenderLine(
            glm::vec3(kArenaPosition.x, kGroundY, kArenaPosition.z),
            glm::vec3(kArenaPosition.x + kArenaSize.x, kGroundY, kArenaPosition.z),
            glm::vec3(0.0f), glm::vec3(0.0f),
            glm::vec3(0.25f, 0.55f, 0.85f),
            0.7f, 2.0f,
            CosmicEngine::ViewType::Projection);
        RS_MN.RenderLine(
            glm::vec3(kArenaPosition.x + kArenaSize.x, kGroundY, kArenaPosition.z),
            glm::vec3(kArenaPosition.x + kArenaSize.x, kGroundY, kArenaPosition.z + kArenaSize.z),
            glm::vec3(0.0f), glm::vec3(0.0f),
            glm::vec3(0.25f, 0.55f, 0.85f),
            0.7f, 2.0f,
            CosmicEngine::ViewType::Projection);
        RS_MN.RenderLine(
            glm::vec3(kArenaPosition.x + kArenaSize.x, kGroundY, kArenaPosition.z + kArenaSize.z),
            glm::vec3(kArenaPosition.x, kGroundY, kArenaPosition.z + kArenaSize.z),
            glm::vec3(0.0f), glm::vec3(0.0f),
            glm::vec3(0.25f, 0.55f, 0.85f),
            0.7f, 2.0f,
            CosmicEngine::ViewType::Projection);
        RS_MN.RenderLine(
            glm::vec3(kArenaPosition.x, kGroundY, kArenaPosition.z + kArenaSize.z),
            glm::vec3(kArenaPosition.x, kGroundY, kArenaPosition.z),
            glm::vec3(0.0f), glm::vec3(0.0f),
            glm::vec3(0.25f, 0.55f, 0.85f),
            0.7f, 2.0f,
            CosmicEngine::ViewType::Projection);

        if (hasPreviewSpawn || hasPendingSpawn)
        {
            glm::vec3 markerPosition = hasPendingSpawn ? pendingSpawnPosition : previewSpawnPosition;
            glm::vec3 markerColor = hasPendingSpawn ? glm::vec3(1.0f, 0.75f, 0.2f) : glm::vec3(0.25f, 0.95f, 0.95f);
            RS_MN.RenderParallelepipedLines(
                markerPosition,
                glm::vec3(1.2f, 0.15f, 1.2f),
                glm::vec3(0.0f),
                glm::vec3(0.0f),
                markerColor,
                0.9f,
                2.0f,
                false,
                CosmicEngine::ViewType::Projection);
        }
    }
}

void NetworkDemoScene::update(double deltaTime)
{
    float dt = static_cast<float>(deltaTime);

    // Always process network messages
    NET_MN.ProcessMessages();

    switch (currentState)
    {
    case GameState::Menu:
    {
        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
        }
        break;
    }

    case GameState::Connecting:
    {
        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
        }
        break;
    }

    case GameState::Playing:
    {
        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
            break;
        }

        hasPreviewSpawn = TryGetMouseArenaPosition(previewSpawnPosition);

        pendingSpawnTimer -= dt;
        if (hasPendingSpawn && pendingSpawnTimer <= 0.0f)
        {
            SpawnLocalProjectile(pendingSpawnPosition);
            hasPendingSpawn = false;
            pendingSpawnTimer = 0.0f;
            spawnCooldown = kSpawnCooldownTime;
        }

        // Spawn projectile with 'E' key
        spawnCooldown -= dt;
        if (spawnCooldown <= 0.0f && !hasPendingSpawn && hasPreviewSpawn &&
            INP_MN.IsMouseButtonPressed(0, CosmicEngine::KeyDown) && !UI_MN.IsMouseHoverAny())
        {
            hasPendingSpawn = true;
            pendingSpawnTimer = 0.5f;
            pendingSpawnPosition = previewSpawnPosition;
        }

        // Send player state at network tick rate
        stateUpdateTimer += dt;
        if (stateUpdateTimer >= kStateUpdateInterval)
        {
            SendPlayerState();
            stateUpdateTimer = 0.0f;
        }

        // Check projectile-player collisions (server authority)
        if (NET_MN.IsServer())
        {
            CheckProjectileCollisions();
        }

        UpdateHUD();

        // F11 fullscreen toggle
        if (INP_MN.IsKeyPressed(GLFW_KEY_F11, CosmicEngine::KeyDown))
        {
            GM_MN.toggleFullscreen();
        }
        break;
    }

    case GameState::GameOver:
    {
        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
        }
        break;
    }
    }
}

// ══════════════════════════════════════════════════
// UI Setup
// ══════════════════════════════════════════════════

void NetworkDemoScene::SetupMenuUI()
{
    float centerX = 960.0f;

    txtTitle = new CosmicEngine::UIText(
        "NETWORK DEMO",
        "demo_font_large",
        glm::vec2(centerX - 250.0f, 120.0f),
        glm::vec2(500.0f, 60.0f),
        true);
    txtTitle->SetTextColor(glm::vec3(0.3f, 0.8f, 1.0f));
    UI_MN.AddElement(txtTitle);

    txtInstructions = new CosmicEngine::UIText(
        "Choose role to start",
        "demo_font",
        glm::vec2(centerX - 250.0f, 200.0f),
        glm::vec2(500.0f, 40.0f),
        true);
    txtInstructions->SetTextColor(glm::vec3(0.8f, 0.8f, 0.8f));
    UI_MN.AddElement(txtInstructions);

    btnServer = new CosmicEngine::UIButton(
        "HOST SERVER",
        "demo_font",
        "",
        glm::vec2(centerX - 150.0f, 320.0f),
        glm::vec2(300.0f, 60.0f),
        true, true);
    btnServer->SetOnClick([this]()
                          { StartAsServer(); });
    UI_MN.AddElement(btnServer);

    btnClient = new CosmicEngine::UIButton(
        "JOIN AS CLIENT",
        "demo_font",
        "",
        glm::vec2(centerX - 150.0f, 420.0f),
        glm::vec2(300.0f, 60.0f),
        true, true);
    btnClient->SetOnClick([this]()
                          { SwitchToClientConnect(); });
    UI_MN.AddElement(btnClient);
}

void NetworkDemoScene::SetupClientConnectionUI()
{
    float centerX = 960.0f;

    txtFieldIp = new CosmicEngine::UITextField(
        "127.0.0.1",
        "Server IP",
        "demo_font",
        glm::vec2(centerX - 200.0f, 300.0f),
        glm::vec2(400.0f, 45.0f),
        64, false);
    UI_MN.AddElement(txtFieldIp);

    txtFieldPort = new CosmicEngine::UITextField(
        "25565",
        "Port",
        "demo_font",
        glm::vec2(centerX - 200.0f, 370.0f),
        glm::vec2(400.0f, 45.0f),
        8, false);
    UI_MN.AddElement(txtFieldPort);

    btnConnect = new CosmicEngine::UIButton(
        "CONNECT",
        "demo_font",
        "",
        glm::vec2(centerX - 100.0f, 450.0f),
        glm::vec2(200.0f, 50.0f),
        true, false);
    btnConnect->SetOnClick([this]()
                           { StartAsClient(); });
    UI_MN.AddElement(btnConnect);

    btnBack = new CosmicEngine::UIButton(
        "BACK",
        "demo_font",
        "",
        glm::vec2(centerX - 100.0f, 530.0f),
        glm::vec2(200.0f, 50.0f),
        true, false);
    btnBack->SetOnClick([this]()
                        { SwitchToMenu(); });
    UI_MN.AddElement(btnBack);

    txtConnectionStatus = new CosmicEngine::UIText(
        "",
        "demo_font",
        glm::vec2(centerX - 250.0f, 610.0f),
        glm::vec2(500.0f, 40.0f),
        false);
    txtConnectionStatus->SetTextColor(glm::vec3(1.0f, 0.4f, 0.4f));
    UI_MN.AddElement(txtConnectionStatus);
}

void NetworkDemoScene::SetupInGameHUD()
{
    txtHudHealth = new CosmicEngine::UIText(
        "HP: 20/20",
        "demo_font",
        glm::vec2(20.0f, 20.0f),
        glm::vec2(300.0f, 36.0f),
        false);
    txtHudHealth->SetTextColor(glm::vec3(0.3f, 1.0f, 0.3f));
    UI_MN.AddElement(txtHudHealth);

    txtHudInfo = new CosmicEngine::UIText(
        "",
        "demo_font",
        glm::vec2(20.0f, 58.0f),
        glm::vec2(600.0f, 36.0f),
        false);
    txtHudInfo->SetTextColor(glm::vec3(0.8f, 0.8f, 0.3f));
    UI_MN.AddElement(txtHudInfo);

    txtHudControls = new CosmicEngine::UIText(
        "WASD Move | Space Jump | E Spawn Hazard At Mouse",
        "demo_font",
        glm::vec2(20.0f, 1040.0f),
        glm::vec2(900.0f, 36.0f),
        false);
    txtHudControls->SetTextColor(glm::vec3(0.6f, 0.6f, 0.6f));
    UI_MN.AddElement(txtHudControls);
}

void NetworkDemoScene::SetupGameOverUI()
{
    float centerX = 960.0f;

    txtGameOver = new CosmicEngine::UIText(
        "GAME OVER",
        "demo_font_large",
        glm::vec2(centerX - 300.0f, 350.0f),
        glm::vec2(600.0f, 60.0f),
        false);
    txtGameOver->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
    UI_MN.AddElement(txtGameOver);

    btnReturnMenu = new CosmicEngine::UIButton(
        "RETURN TO MENU",
        "demo_font",
        "",
        glm::vec2(centerX - 150.0f, 480.0f),
        glm::vec2(300.0f, 60.0f),
        true, false);
    btnReturnMenu->SetOnClick([this]()
                              {
                                  CleanupGame();
                                  SwitchToMenu();
                              });
    UI_MN.AddElement(btnReturnMenu);
}

void NetworkDemoScene::HideAllUI()
{
    // Menu
    if (txtTitle) txtTitle->SetVisible(false);
    if (txtInstructions) txtInstructions->SetVisible(false);
    if (btnServer) btnServer->SetVisible(false);
    if (btnClient) btnClient->SetVisible(false);

    // Client connect
    if (txtFieldIp) txtFieldIp->SetVisible(false);
    if (txtFieldPort) txtFieldPort->SetVisible(false);
    if (txtFieldIp) txtFieldIp->SetFocused(false);
    if (txtFieldPort) txtFieldPort->SetFocused(false);
    if (btnConnect) btnConnect->SetVisible(false);
    if (btnBack) btnBack->SetVisible(false);
    if (txtConnectionStatus) txtConnectionStatus->SetVisible(false);

    // HUD
    if (txtHudHealth) txtHudHealth->SetVisible(false);
    if (txtHudInfo) txtHudInfo->SetVisible(false);
    if (txtHudControls) txtHudControls->SetVisible(false);

    // Game Over
    if (txtGameOver) txtGameOver->SetVisible(false);
    if (btnReturnMenu) btnReturnMenu->SetVisible(false);
}

// ══════════════════════════════════════════════════
// State transitions
// ══════════════════════════════════════════════════

void NetworkDemoScene::SwitchToMenu()
{
    currentState = GameState::Menu;
    HideAllUI();

    txtTitle->SetVisible(true);
    txtInstructions->SetVisible(true);
    btnServer->SetVisible(true);
    btnClient->SetVisible(true);

    // Show cursor in menu
    glfwSetInputMode(GM_MN.getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;
}

void NetworkDemoScene::SwitchToClientConnect()
{
    currentState = GameState::Connecting;
    HideAllUI();

    txtTitle->SetVisible(true);
    txtFieldIp->SetVisible(true);
    txtFieldPort->SetVisible(true);
    btnConnect->SetVisible(true);
    btnBack->SetVisible(true);
    txtConnectionStatus->SetVisible(true);
    txtConnectionStatus->SetText("Enter server IP and port");
    txtConnectionStatus->SetTextColor(glm::vec3(0.8f, 0.8f, 0.8f));

    glfwSetInputMode(GM_MN.getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;
}

void NetworkDemoScene::SwitchToPlaying()
{
    currentState = GameState::Playing;
    HideAllUI();

    txtHudHealth->SetVisible(true);
    txtHudInfo->SetVisible(true);
    txtHudControls->SetVisible(true);

    ConfigureFixedCamera();
    glfwSetInputMode(GM_MN.getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;
    if (cameraController)
    {
        cameraController->SetMouseCaptureEnabled(false);
    }

    stateUpdateTimer = 0.0f;
    spawnCooldown = 0.0f;
    pendingSpawnTimer = 0.0f;
    hasPendingSpawn = false;
    hasPreviewSpawn = false;

    RegisterNetworkHandlers();
    UpdateHUD();
}

void NetworkDemoScene::SwitchToGameOver(const std::string &message)
{
    currentState = GameState::GameOver;
    HideAllUI();

    txtGameOver->SetText(message);
    txtGameOver->SetVisible(true);
    btnReturnMenu->SetVisible(true);

    glfwSetInputMode(GM_MN.getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;
    hasPendingSpawn = false;
    hasPreviewSpawn = false;
    if (cameraController)
        cameraController->SetMouseCaptureEnabled(false);
}

// ══════════════════════════════════════════════════
// Network: start roles
// ══════════════════════════════════════════════════

void NetworkDemoScene::StartAsServer()
{
    if (NET_MN.StartServer())
    {
        // Create local player for the server host
        auto *localPlayer = new NetworkDemoPlayer(0, "Host", RandomSpawnPosition(), glm::vec3(2.0f), true);
        OBJ_MN.Add(localPlayer);
        players[0] = localPlayer;

        SwitchToPlaying();
        std::cout << "[NetworkDemoScene] Started as server." << std::endl;
    }
    else
    {
        txtInstructions->SetText("Failed to start server!");
        txtInstructions->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
    }
}

void NetworkDemoScene::StartAsClient()
{
    std::string ip = txtFieldIp->GetText();
    std::string portStr = txtFieldPort->GetText();

    if (ip.empty())
    {
        txtConnectionStatus->SetText("Please enter a server IP");
        txtConnectionStatus->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
        return;
    }

    int port = 25565;
    try
    {
        port = std::stoi(portStr);
    }
    catch (...)
    {
        txtConnectionStatus->SetText("Invalid port number");
        txtConnectionStatus->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
        return;
    }

    txtConnectionStatus->SetText("Connecting...");
    txtConnectionStatus->SetTextColor(glm::vec3(1.0f, 1.0f, 0.3f));

    if (NET_MN.ConnectToServer(ip, port, "Player"))
    {
        // Local player will be created when we receive AssignClientId
        SwitchToPlaying();
        std::cout << "[NetworkDemoScene] Connected to " << ip << ":" << port << std::endl;
    }
    else
    {
        txtConnectionStatus->SetText("Connection failed!");
        txtConnectionStatus->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
    }
}

// ══════════════════════════════════════════════════
// Network: handlers
// ══════════════════════════════════════════════════

void NetworkDemoScene::RegisterNetworkHandlers()
{
    NET_MN.ClearAllHandlers();

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::AssignClientId,
                           [this](int s, const nlohmann::json &p)
                           { OnAssignClientId(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::ClientConnect,
                           [this](int s, const nlohmann::json &p)
                           { OnClientConnect(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::ClientDisconnect,
                           [this](int s, const nlohmann::json &p)
                           { OnClientDisconnect(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::PlayerState,
                           [this](int s, const nlohmann::json &p)
                           { OnPlayerState(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::SpawnObject,
                           [this](int s, const nlohmann::json &p)
                           { OnSpawnObject(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::DamageEvent,
                           [this](int s, const nlohmann::json &p)
                           { OnDamageEvent(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::PlayerDeath,
                           [this](int s, const nlohmann::json &p)
                           { OnPlayerDeath(s, p); });

    NET_MN.RegisterHandler(CosmicEngine::NetMessageType::MatchEnded,
                           [this](int s, const nlohmann::json &p)
                           { OnMatchEnded(s, p); });
}

void NetworkDemoScene::UnregisterNetworkHandlers()
{
    NET_MN.ClearAllHandlers();
}

void NetworkDemoScene::OnAssignClientId(int senderId, const nlohmann::json &payload)
{
    (void)senderId;
    int clientId = payload.value("clientId", -1);
    if (clientId < 0)
        return;

    std::cout << "[NetworkDemoScene] Assigned client ID: " << clientId << std::endl;

    // Create local player
    if (players.find(clientId) == players.end())
    {
        auto *localPlayer = new NetworkDemoPlayer(clientId, "Player " + std::to_string(clientId),
                                                   RandomSpawnPosition(), glm::vec3(2.0f), true);
        OBJ_MN.Add(localPlayer);
        players[clientId] = localPlayer;
    }
}

void NetworkDemoScene::OnClientConnect(int senderId, const nlohmann::json &payload)
{
    int clientId = payload.value("clientId", -1);
    if (clientId < 0)
        return;

    // Don't create duplicate for self
    int localId = NET_MN.GetLocalClientId();
    if (clientId == localId)
        return;

    (void)senderId;

    std::cout << "[NetworkDemoScene] Player " << clientId << " connected." << std::endl;
    GetOrCreatePlayer(clientId);
}

void NetworkDemoScene::OnClientDisconnect(int senderId, const nlohmann::json &payload)
{
    (void)senderId;
    int clientId = payload.value("clientId", -1);

    if (currentState == GameState::Playing && NET_MN.IsServer() && clientId >= 0)
    {
        BroadcastMatchEnded("disconnect", -1, -1, clientId);
        return;
    }

    if (currentState == GameState::Playing && NET_MN.IsClient() && clientId == NET_MN.GetLocalClientId())
    {
        EndMatch("HOST DISCONNECTED");
        return;
    }

    auto it = players.find(clientId);
    if (it != players.end())
    {
        OBJ_MN.Remove(it->second->GetID());
        players.erase(it);
        std::cout << "[NetworkDemoScene] Player " << clientId << " disconnected." << std::endl;
    }
}

void NetworkDemoScene::OnPlayerState(int senderId, const nlohmann::json &payload)
{
    int netId = payload.value("senderId", senderId);
    int localId = NET_MN.GetLocalClientId();

    // Don't update self from network
    if (netId == localId)
        return;

    NetworkDemoPlayer *player = GetOrCreatePlayer(netId);
    if (!player)
        return;

    if (payload.contains("x"))
    {
        glm::vec3 pos(
            payload.value("x", 0.0f),
            payload.value("y", 0.0f),
            payload.value("z", 0.0f));
        player->SetPosition(pos);
    }

    if (payload.contains("health"))
    {
        player->SetHealth(payload.value("health", 20));
    }
}

void NetworkDemoScene::OnSpawnObject(int senderId, const nlohmann::json &payload)
{
    (void)senderId;
    int ownerId = payload.value("senderId", -1);
    int objId = payload.value("objectId", 0);
    int damage = payload.value("damage", 1);
    float px = payload.value("x", 0.0f);
    float py = payload.value("y", 0.0f);
    float pz = payload.value("z", 0.0f);

    // Don't double-spawn our own projectiles
    int localId = NET_MN.GetLocalClientId();
    if (ownerId == localId)
        return;

    auto *projectile = new NetworkDemoProjectile(
        ownerId, objId, damage,
        glm::vec3(px, py, pz),
        glm::vec3(1.2f),
        5.0f);
    OBJ_MN.Add(projectile);
}

void NetworkDemoScene::OnDamageEvent(int senderId, const nlohmann::json &payload)
{
    (void)senderId;
    int targetId = payload.value("targetId", -1);
    int damage = payload.value("damage", 1);

    auto it = players.find(targetId);
    if (it != players.end() && it->second)
    {
        it->second->TakeDamage(damage);

        if (!it->second->IsAlivePlayer())
        {
            // Notify death
            if (NET_MN.IsServer())
            {
                CosmicEngine::NetworkMessage deathMsg(
                    CosmicEngine::NetMessageType::PlayerDeath,
                    {{"deadPlayerId", targetId}});
                NET_MN.Broadcast(deathMsg);
                OnPlayerDeath(-1, deathMsg.payload);
            }
        }
    }
}

void NetworkDemoScene::OnPlayerDeath(int senderId, const nlohmann::json &payload)
{
    (void)senderId;
    (void)payload;
}

void NetworkDemoScene::OnMatchEnded(int senderId, const nlohmann::json &payload)
{
    (void)senderId;

    if (currentState != GameState::Playing)
    {
        return;
    }

    const std::string reason = payload.value("reason", std::string());
    const int winnerId = payload.value("winnerId", -1);
    const int loserId = payload.value("loserId", -1);
    const int disconnectedId = payload.value("disconnectedId", -1);
    const int localId = NET_MN.GetLocalClientId();

    if (reason == "disconnect")
    {
        if (NET_MN.IsClient())
        {
            EndMatch("HOST DISCONNECTED");
        }
        else if (disconnectedId == localId)
        {
            EndMatch("YOU DISCONNECTED");
        }
        else
        {
            EndMatch("OPPONENT DISCONNECTED");
        }
        return;
    }

    if (reason == "elimination")
    {
        if (loserId == localId)
        {
            EndMatch("YOU DIED!");
        }
        else if (winnerId == localId)
        {
            EndMatch("YOU WIN!");
        }
        else
        {
            EndMatch("MATCH FINISHED");
        }
    }
}

// ══════════════════════════════════════════════════
// Game logic
// ══════════════════════════════════════════════════

void NetworkDemoScene::SpawnLocalProjectile(const glm::vec3 &spawnPos)
{
    NetworkDemoPlayer *local = GetLocalPlayer();
    if (!local || !local->IsAlivePlayer())
        return;

    int localId = NET_MN.GetLocalClientId();
    int damage = RandomDamage();

    int projId = ++nextProjectileId;

    auto *projectile = new NetworkDemoProjectile(
        localId, projId, damage,
        spawnPos,
        glm::vec3(1.2f),
        5.0f);
    OBJ_MN.Add(projectile);

    // Broadcast to other players
    CosmicEngine::NetworkMessage msg(CosmicEngine::NetMessageType::SpawnObject, {
        {"objectId", projId},
        {"damage", damage},
        {"x", spawnPos.x},
        {"y", spawnPos.y},
        {"z", spawnPos.z}
    });

    if (NET_MN.IsServer())
        NET_MN.Broadcast(msg);
    else
        NET_MN.SendToServer(msg);
}

void NetworkDemoScene::SendPlayerState()
{
    NetworkDemoPlayer *local = GetLocalPlayer();
    if (!local)
        return;

    glm::vec3 pos = local->GetPosition();
    CosmicEngine::NetworkMessage msg(CosmicEngine::NetMessageType::PlayerState, {
        {"x", pos.x},
        {"y", pos.y},
        {"z", pos.z},
        {"health", local->GetHealth()}
    });

    if (NET_MN.IsServer())
        NET_MN.Broadcast(msg);
    else
        NET_MN.SendToServer(msg);
}

void NetworkDemoScene::UpdateHUD()
{
    NetworkDemoPlayer *local = GetLocalPlayer();
    if (!local)
        return;

    if (txtHudHealth)
    {
        std::string healthStr = "HP: " + std::to_string(local->GetHealth()) + "/" + std::to_string(local->GetMaxHealth());
        txtHudHealth->SetText(healthStr);

        float ratio = static_cast<float>(local->GetHealth()) / static_cast<float>(local->GetMaxHealth());
        if (ratio > 0.5f)
            txtHudHealth->SetTextColor(glm::vec3(0.3f, 1.0f, 0.3f));
        else if (ratio > 0.25f)
            txtHudHealth->SetTextColor(glm::vec3(1.0f, 1.0f, 0.3f));
        else
            txtHudHealth->SetTextColor(glm::vec3(1.0f, 0.3f, 0.3f));
    }

    if (txtHudInfo)
    {
        std::string role = NET_MN.IsServer() ? "Server" : "Client";
        std::string info = role + " | ID: " + std::to_string(NET_MN.GetLocalClientId()) +
                           " | Players: " + std::to_string(players.size());
        txtHudInfo->SetText(info);
    }
}

void NetworkDemoScene::CheckProjectileCollisions()
{
    // Only server checks collisions and sends damage events
    auto projectiles = OBJ_MN.FindByClassName(NetworkDemoProjectile::StaticClassName());

    for (auto *obj : projectiles)
    {
        auto *projectile = static_cast<NetworkDemoProjectile *>(obj);
        if (!projectile || !projectile->IsAlive())
            continue;

        glm::vec3 projPos = projectile->GetPosition();
        glm::vec3 projSize = projectile->GetSize();

        for (auto &[netId, player] : players)
        {
            if (!player || !player->IsAlivePlayer())
                continue;

            // Don't damage the owner
            if (netId == projectile->GetOwnerNetworkId())
                continue;

            glm::vec3 playerPos = player->GetPosition();
            glm::vec3 playerSize = player->GetSize();

            // AABB collision check
            bool collides =
                projPos.x < playerPos.x + playerSize.x &&
                projPos.x + projSize.x > playerPos.x &&
                projPos.y < playerPos.y + playerSize.y &&
                projPos.y + projSize.y > playerPos.y &&
                projPos.z < playerPos.z + playerSize.z &&
                projPos.z + projSize.z > playerPos.z;

            if (collides)
            {
                int damage = projectile->GetDamage();
                player->TakeDamage(damage);

                // Broadcast damage event
                CosmicEngine::NetworkMessage dmgMsg(CosmicEngine::NetMessageType::DamageEvent, {
                    {"targetId", netId},
                    {"damage", damage},
                    {"sourceId", projectile->GetOwnerNetworkId()}
                });
                NET_MN.Broadcast(dmgMsg);

                // Destroy projectile
                projectile->Destroy();

                // Check death
                if (!player->IsAlivePlayer())
                {
                    CosmicEngine::NetworkMessage deathMsg(
                        CosmicEngine::NetMessageType::PlayerDeath,
                        {{"deadPlayerId", netId}});
                    NET_MN.Broadcast(deathMsg);
                    BroadcastMatchEnded("elimination", projectile->GetOwnerNetworkId(), netId, -1);
                }

                break; // One hit per projectile
            }
        }
    }
}

void NetworkDemoScene::ConfigureFixedCamera()
{
    const glm::vec3 arenaCenter = kArenaPosition + kArenaSize * 0.5f;
    const glm::vec3 cameraPosition = arenaCenter + glm::vec3(0.0f, 50.0f, 50.0f); //+ glm::vec3(0.0f, kArenaSize.y * 0.85f, kArenaSize.z * 0.78f);
    const glm::vec3 lookTarget = arenaCenter + glm::vec3(0.0f, -20.0f, 0.0f); //+ glm::vec3(0.0f, kArenaSize.y * 0.25f, 0.0f);

    CAM_MN.reset();
    CAM_MN.SetZoom(100.0f);
    CAM_MN.SetFocusPosition(cameraPosition, lookTarget);
}

bool NetworkDemoScene::TryGetMouseArenaPosition(glm::vec3 &worldPosition) const
{
    glm::vec2 mousePosition = INP_MN.GetMousePosition_UI();
    glm::vec2 baseWindowSize = CAM_MN.GetBaseWindowSize();

    if (baseWindowSize.x <= 0.0f || baseWindowSize.y <= 0.0f)
    {
        return false;
    }

    float ndcX = (mousePosition.x / baseWindowSize.x) * 2.0f - 1.0f;
    float ndcY = 1.0f - (mousePosition.y / baseWindowSize.y) * 2.0f;

    glm::mat4 inverseViewProjection = glm::inverse(CAM_MN.GetProjectionMatrix() * CAM_MN.GetViewMatrix());
    glm::vec4 nearPoint = inverseViewProjection * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farPoint = inverseViewProjection * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    if (std::abs(nearPoint.w) <= 0.0001f || std::abs(farPoint.w) <= 0.0001f)
    {
        return false;
    }

    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    glm::vec3 rayOrigin = glm::vec3(nearPoint);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(farPoint - nearPoint));

    if (std::abs(rayDirection.y) <= 0.0001f)
    {
        return false;
    }

    float distanceToGround = (kGroundY - rayOrigin.y) / rayDirection.y;
    if (distanceToGround < 0.0f)
    {
        return false;
    }

    glm::vec3 hitPoint = rayOrigin + rayDirection * distanceToGround;
    glm::vec3 projectileSize(1.2f);
    glm::vec3 maxPosition = kArenaPosition + kArenaSize - projectileSize;

    worldPosition.x = std::clamp(hitPoint.x, kArenaPosition.x, maxPosition.x);
    worldPosition.y = kGroundY;
    worldPosition.z = std::clamp(hitPoint.z, kArenaPosition.z, maxPosition.z);
    return true;
}

NetworkDemoPlayer *NetworkDemoScene::GetLocalPlayer()
{
    int localId = NET_MN.GetLocalClientId();
    auto it = players.find(localId);
    if (it != players.end())
        return it->second;
    return nullptr;
}

NetworkDemoPlayer *NetworkDemoScene::GetOrCreatePlayer(int networkId)
{
    auto it = players.find(networkId);
    if (it != players.end())
        return it->second;

    // Create remote player
    auto *player = new NetworkDemoPlayer(
        networkId,
        "Player " + std::to_string(networkId),
        RandomSpawnPosition(),
        glm::vec3(2.0f),
        false);
    OBJ_MN.Add(player);
    players[networkId] = player;
    return player;
}

void NetworkDemoScene::CleanupGame()
{
    UnregisterNetworkHandlers();
    NET_MN.Disconnect();
    ClearGameplayObjects();
    players.clear();
    nextProjectileId = 0;
    stateUpdateTimer = 0.0f;
    spawnCooldown = 0.0f;
    pendingSpawnTimer = 0.0f;
    hasPendingSpawn = false;
    hasPreviewSpawn = false;

    if (txtConnectionStatus)
    {
        txtConnectionStatus->SetText("");
    }

    // Objects and UI are cleaned up by Scene::Clear
}

void NetworkDemoScene::ClearGameplayObjects()
{
    auto playersToRemove = OBJ_MN.FindByClassName(NetworkDemoPlayer::StaticClassName());
    for (auto *object : playersToRemove)
    {
        if (object)
        {
            OBJ_MN.Remove(object->GetID());
        }
    }

    auto projectilesToRemove = OBJ_MN.FindByClassName(NetworkDemoProjectile::StaticClassName());
    for (auto *object : projectilesToRemove)
    {
        if (object)
        {
            OBJ_MN.Remove(object->GetID());
        }
    }
}

void NetworkDemoScene::EndMatch(const std::string &message)
{
    ClearGameplayObjects();
    players.clear();
    nextProjectileId = 0;
    stateUpdateTimer = 0.0f;
    spawnCooldown = 0.0f;
    pendingSpawnTimer = 0.0f;
    hasPendingSpawn = false;
    hasPreviewSpawn = false;
    SwitchToGameOver(message);
}

void NetworkDemoScene::BroadcastMatchEnded(const std::string &reason, int winnerId, int loserId, int disconnectedId)
{
    CosmicEngine::NetworkMessage matchMsg(
        CosmicEngine::NetMessageType::MatchEnded,
        {
            {"reason", reason},
            {"winnerId", winnerId},
            {"loserId", loserId},
            {"disconnectedId", disconnectedId}
        });

    if (NET_MN.IsServer())
    {
        NET_MN.Broadcast(matchMsg);
        OnMatchEnded(-1, matchMsg.payload);
    }
    else if (NET_MN.IsClient())
    {
        NET_MN.SendToServer(matchMsg);
    }
}

#endif // GAME_3D_CONFIGURATION
