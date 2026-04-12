#ifndef NETWORK_DEMO_SCENE_HPP
#define NETWORK_DEMO_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/managers/network/network_message.hpp>

#include <string>
#include <map>
#include <cstdint>

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    #include <CosmicEngine/controllers/camera/classic_3d_camera_controller.hpp>
    #include <memory>
#endif

namespace CosmicEngine
{
    class UIButton;
    class UIText;
    class UITextField;
}

class NetworkDemoPlayer;

/**
 * @brief Network multiplayer demo scene.
 * 
 * Demonstrates:
 * - Server/Client selection UI
 * - Connection via IP:Port text field
 * - Networked player movement
 * - Spawning hazard objects that damage other players
 * - Health system (20 HP per player, 1-3 damage per hazard, 5s lifetime)
 */
class NetworkDemoScene : public CosmicEngine::Scene
{
private:
    // ─── Game state ─────────────────────────────
    enum class GameState
    {
        Menu,           // Role selection screen
        Connecting,     // Waiting for connection
        Playing,        // In-game
        GameOver        // Someone died
    };

    GameState currentState;
    int nextProjectileId;
    float stateUpdateTimer;
    float spawnCooldown;
    float pendingSpawnTimer;
    bool hasPendingSpawn;
    bool hasPreviewSpawn;
    glm::vec3 pendingSpawnPosition;
    glm::vec3 previewSpawnPosition;
    static constexpr float kStateUpdateInterval = 1.0f / 20.0f; // 20Hz network tick
    static constexpr float kSpawnCooldownTime = 0.5f;

    // ─── UI elements ────────────────────────────
    // Menu
    CosmicEngine::UIButton *btnServer;
    CosmicEngine::UIButton *btnClient;
    CosmicEngine::UIText *txtTitle;
    CosmicEngine::UIText *txtInstructions;

    // Client connection
    CosmicEngine::UITextField *txtFieldIp;
    CosmicEngine::UITextField *txtFieldPort;
    CosmicEngine::UIButton *btnConnect;
    CosmicEngine::UIButton *btnBack;
    CosmicEngine::UIText *txtConnectionStatus;

    // In-game HUD
    CosmicEngine::UIText *txtHudHealth;
    CosmicEngine::UIText *txtHudInfo;
    CosmicEngine::UIText *txtHudControls;

    // Game over
    CosmicEngine::UIText *txtGameOver;
    CosmicEngine::UIButton *btnReturnMenu;

    // ─── Network state ──────────────────────────
    std::map<int, NetworkDemoPlayer *> players;

    // ─── Camera ─────────────────────────────────
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    std::unique_ptr<CosmicEngine::Classic3DCameraController> cameraController;
    bool mouseCaptured;
#endif

    // ─── Setup methods ──────────────────────────
    void SetupMenuUI();
    void SetupClientConnectionUI();
    void SetupInGameHUD();
    void SetupGameOverUI();
    void HideAllUI();

    void SwitchToMenu();
    void SwitchToClientConnect();
    void SwitchToPlaying();
    void SwitchToGameOver(const std::string &message);

    // ─── Network handlers ───────────────────────
    void RegisterNetworkHandlers();
    void UnregisterNetworkHandlers();
    void OnAssignClientId(int senderId, const nlohmann::json &payload);
    void OnClientConnect(int senderId, const nlohmann::json &payload);
    void OnClientDisconnect(int senderId, const nlohmann::json &payload);
    void OnPlayerState(int senderId, const nlohmann::json &payload);
    void OnSpawnObject(int senderId, const nlohmann::json &payload);
    void OnDamageEvent(int senderId, const nlohmann::json &payload);
    void OnPlayerDeath(int senderId, const nlohmann::json &payload);
    void OnMatchEnded(int senderId, const nlohmann::json &payload);

    // ─── Game logic ─────────────────────────────
    void StartAsServer();
    void StartAsClient();
    void SpawnLocalProjectile(const glm::vec3 &spawnPosition);
    void SendPlayerState();
    void UpdateHUD();
    void CheckProjectileCollisions();
    void ConfigureFixedCamera();
    bool TryGetMouseArenaPosition(glm::vec3 &worldPosition) const;
    void ClearGameplayObjects();
    void EndMatch(const std::string &message);
    void BroadcastMatchEnded(const std::string &reason, int winnerId = -1, int loserId = -1, int disconnectedId = -1);

    NetworkDemoPlayer *GetLocalPlayer();
    NetworkDemoPlayer *GetOrCreatePlayer(int networkId);

    void CleanupGame();

public:
    NetworkDemoScene();

    void init() override;
    void reset() override;
    void draw() override;
    void update(double deltaTime) override;
    void loadResources() override;
};

#endif // NETWORK_DEMO_SCENE_HPP
