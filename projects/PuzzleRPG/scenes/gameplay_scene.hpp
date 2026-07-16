#ifndef PUZZLERPG_GAMEPLAY_SCENE_HPP
#define PUZZLERPG_GAMEPLAY_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/ui_element.hpp>

#include "../entities/puzzle_player.hpp"
#include "../systems/dialog_box.hpp"
#include "../systems/level_data.hpp"
#include "../systems/level_runtime.hpp"

#include <nlohmann/json_fwd.hpp>
#include <memory>
#include <vector>

namespace CosmicEngine { class Object; }
namespace CosmicEngine { class UIImage; }
namespace CosmicEngine { class Light; }

namespace PuzzleRPG
{
    // Escena principal del juego. Construye el LevelRuntime a partir del nivel
    // seleccionado en GameSession y conecta input + UI + (opcional) red.
    class GameplayScene : public CosmicEngine::Scene
    {
    public:
        enum class PlayState { Playing, Paused, ConnectionLost, Completed };

        GameplayScene();
        ~GameplayScene() override;
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        std::unique_ptr<LevelRuntime> runtime;
        std::unique_ptr<DialogBox>    dialog;
        LevelData                     levelData;
        PuzzlePlayer                 *localPlayer;
        PuzzlePlayer                 *remotePlayer;
        CosmicEngine::UIText         *hudLabel;
        // Single-slot inventory UI: base cell image and optional overlay sprite
        CosmicEngine::UIImage       *inventoryBaseImage;
        CosmicEngine::UIText        *inventorySlotLabel;
        CosmicEngine::UIElement     *inventoryOverlay;
        // Dialog message UI (background image + text)
        CosmicEngine::UIImage       *dialogMessageBg;
        CosmicEngine::UIText        *dialogMessageText;
        // We draw the overlay sprite manually in UpdateHud using registered clips.
        glm::vec2                    inventoryBasePos;
        glm::vec2                    inventoryBaseSize;
        bool                          networkSetup;
        bool                          waitingForLevel;
        bool                          levelBuilt;

        // Pausa / desconexion.
        PlayState                                  state;
        std::vector<CosmicEngine::UIElement *>     pauseOverlay;
        std::vector<CosmicEngine::UIElement *>     lostOverlay;
        std::vector<CosmicEngine::UIElement *>     winOverlay;

        // Estado de teletransporte programado (host).
        bool  pendingTeleport;
        float pendingTeleportTimer;
        int   pendingTeleportPlayerIndex;
        Cell  pendingTeleportTarget;
        int   pendingTeleportTeleId;

        // Camara.
        float zoom;
        // Focus indicator (visual shown when something interactable is in front of player)
        CosmicEngine::Object *focusIndicator = nullptr;
        // Previous-frame movement state for local player. Used to detect
        // when a tween finishes so we can apply queued/held movement inputs
        // immediately for fluid movement.
        bool prevLocalPlayerWasMoving = false;
        // Pending facing-change grace period: when the player changes facing
        // we wait up to this timer (seconds) for the key to remain held
        // before initiating the move. If released within the window, only
        // the facing change is applied.
        bool pendingFacingChange = false;
        float pendingFacingTimer = 0.0f;
        int pendingFacingDir = -1;
        const char *pendingFacingAction = nullptr;
        // True while the client has sent Intent_Move and is waiting for the
        // server's Msg_PlayerState confirmation. Prevents duplicate move intents
        // from being queued before the first one is acknowledged.
        bool pendingNetMove = false;
        float focusAnimTimer = 0.0f;
        bool hasFocusedFrontCell = false;
        Cell focusedFrontCell{0, 0};
        glm::vec3 previousAmbientColor = glm::vec3(0.05f);
        glm::vec3 previousAmbientDirection = glm::vec3(0.05f);
        glm::vec3 previousAmbientDiffuse = glm::vec3(0.4f);
        glm::vec3 previousAmbientSpecular = glm::vec3(0.5f);
        bool lightingStateCaptured = false;

        void ApplyInput();
        void SetupNetwork();
        void TeardownNetwork();
        void UpdateHud();
        void UpdateCameraFollow();
        void UpdateYSortLayers();
        void ApplyNightLighting();
        void RestoreLighting();

        void BuildOverlays();
        void SetState(PlayState newState);
        void GoToMainMenu();
        void SendPlayerState(const PuzzlePlayer &player);
        void BroadcastWorldState();
        void ApplyWorldState(const nlohmann::json &payload);
        void ApplyReplicatedPlayerState(int playerIndex, int facing, Cell targetCell);
        void HandleLevelCompleted();
        void OnTeleporterStep(PuzzlePlayer &player, class TeleporterObject &teleporter);

        // Carga del nivel (servidor: desde disco; cliente: por red).
        void BuildLevelAndSpawn(int localPlayerIndex);
    };
}

#endif
