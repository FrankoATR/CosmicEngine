#include "gameplay_scene.hpp"

#include "main_menu_scene.hpp"

#include "../entities/teleporter_object.hpp"
#include "../entities/pickup_object.hpp"
#include "../entities/pushable_object.hpp"
#include "../entities/button_object.hpp"
#include "../entities/door_object.hpp"
#include "../entities/pressure_plate.hpp"
#include "../systems/debug_text.hpp"
#include "../systems/game_session.hpp"
#include "../systems/grid.hpp"
#include "../systems/net_protocol.hpp"
#include "../utilities/puzzle_asset_defines.hpp"
#include "../utilities/puzzle_menu_music.hpp"
#include "../utilities/puzzle_input_actions.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include LIGHTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include <CosmicEngine/managers/camera/camera_manager.hpp>
#include <CosmicEngine/managers/input/input_manager.hpp>
#include <CosmicEngine/managers/network/network_manager.hpp>

#include <GLFW/glfw3.h>

// Windows define DialogBox como macro (DialogBoxA/W) cuando se incluye
// windows.h (a traves de enet/winsock2). La deshacemos para que nuestra
// clase DialogBox sea visible en este archivo.
#ifdef DialogBox
#undef DialogBox
#endif

#include <algorithm>
#include <cmath>
#include <string>
#include <CosmicEngine/utils/log.hpp>

namespace PuzzleRPG
{
    // Small non-networked visual object that draws the focus sprite over a
    // tile. Implemented here to keep rendering order inside ObjectManager.
    class FocusIndicatorObject : public CosmicEngine::Object
    {
    public:
        FocusIndicatorObject(Cell cell)
            : CosmicEngine::Object("FocusIndicator", CellToWorld(cell), glm::vec2(kTileSize, kTileSize), 0.0f, 100),
              cell(cell), frame(0), alpha(1.0f)
        {
        }

        void SetCell(Cell c)
        {
            cell = c;
            SetPosition(CellToWorld(cell));
        }

        void SetFrame(int f) { frame = f; }
        void SetAlpha(float a) { alpha = a; }

        void draw() const override
        {
            if (!GetVisible()) return;
            // Use the dedicated spritesheet key "ui/focus" (loaded by scene).
            CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetUnlit(
                "ui/focus", 0, frame, GetPosition(), GetSize(), 0.0f, glm::vec3(1.0f), alpha, CosmicEngine::ViewType::Ortho);
        }

    private:
        Cell cell;
        int frame;
        float alpha;
    };

    // Lightweight UI element that draws the first frame of a registered
    // animation clip in UI space. We keep it here so it can call
    // DrawClipFirstFrame() directly and be rendered after other UI images.
    class InventoryOverlayElement : public CosmicEngine::UIElement
    {
    public:
        InventoryOverlayElement(glm::vec2 pos, glm::vec2 size, bool visible)
            : UIElement(pos, size, visible, nullptr, CosmicEngine::UIElementType::Image), clipName()
        {
            useSheet = false;
        }

        void SetClipName(const std::string &name)
        {
            clipName = name;
            SetVisible(!clipName.empty());
            useSheet = false;
            RUNTIME_INFO("Inventory overlay clip set: " << clipName);
        }

        void SetSheetVariant(int sheetId, int row, int col)
        {
            sheet = sheetId;
            sheetRow = row;
            sheetCol = col;
            useSheet = (sheet > 0);
            SetVisible(useSheet);
            RUNTIME_INFO("Inventory overlay sheet variant set: sheet=" << sheet << " row=" << row << " col=" << col);
        }

        void draw() override
        {
            if (!IsVisible()) return;

            if (useSheet)
            {
                const char *sheetKey = SheetKeyForSheetId(sheet);
                if (sheetKey && sheetKey[0])
                {
                    // Draw slightly larger than original pixel size (scaled up), centered inside the UI element
                    const float overlayScale = 2.5f; // make sprite a bit larger in the inventory
                    glm::vec2 sizePx = glm::vec2(kTileSize * overlayScale, kTileSize * overlayScale);
                    glm::vec2 center = GetPosition() + GetSize() * 0.5f;
                    glm::vec2 pos = center - sizePx * 0.5f;
                    DrawSheetCell(sheetKey, sheetRow, sheetCol, pos, sizePx, 0.0f, glm::vec3(1.0f), 1.0f, CosmicEngine::ViewType::UI);
                    return;
                }
            }

            if (!clipName.empty())
            {
                const auto *clip = CosmicEngine::AnimationManager::GetInstance().GetClip(clipName.c_str());
                if (clip)
                {
                    const auto *frame = clip->GetFrame(0);
                    if (frame)
                    {
                        const float overlayScale = 2.5f; // make sprite a bit larger in the inventory
                        glm::vec2 sizePx = glm::vec2(kTileSize * overlayScale, kTileSize * overlayScale);
                        glm::vec2 center = GetPosition() + GetSize() * 0.5f;
                        glm::vec2 pos = center - sizePx * 0.5f;
                        const std::string &sheetKey = clip->GetTextureSheetKey();
                        CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetUnlit(
                            sheetKey, frame->row, frame->column, pos, sizePx, 0.0f, glm::vec3(1.0f), 1.0f, CosmicEngine::ViewType::UI);
                        return;
                    }
                }
                // fallback: scaled to element size
                DrawClipFirstFrame(clipName.c_str(), GetPosition(), GetSize(), 0.0f, glm::vec3(1.0f), 1.0f, CosmicEngine::ViewType::UI);
            }
        }

    private:
        std::string clipName;
        bool useSheet;
        int sheet;
        int sheetRow;
        int sheetCol;
    };

    namespace
    {
        // Tiles ahora son de 16 px; subimos el zoom por defecto x4 para que el
        // jugador siga viendose con el mismo tamanyo aparente que antes.
        constexpr float kDefaultZoom = 8.0f;
        constexpr float kZoomMin     = 3.0f;
        constexpr float kZoomMax     = 20.0f;
        constexpr float kZoomStep    = 1.0f;

        float ClampZoom(float value)
        {
            return std::max(kZoomMin, std::min(kZoomMax, value));
        }

        bool TryFaceThenMove(LevelRuntime &runtime, PuzzlePlayer &player, int facing)
        {
            if (player.GetFacing() != facing)
            {
                player.SetFacing(facing);
                return false;
            }

            return runtime.TryMovePlayer(player, facing) != MoveResult::Blocked;
        }
    }

    GameplayScene::GameplayScene()
        : Scene("GameplayScene"),
          runtime(nullptr),
          dialog(nullptr),
          localPlayer(nullptr),
          remotePlayer(nullptr),
          hudLabel(nullptr),
                    inventoryBaseImage(nullptr),
                    inventorySlotLabel(nullptr),
                    inventoryOverlay(nullptr),
                                        dialogMessageBg(nullptr),
                                        dialogMessageText(nullptr),
                    inventoryBasePos(glm::vec2(0.0f)),
                    inventoryBaseSize(glm::vec2(0.0f)),
          networkSetup(false),
          waitingForLevel(false),
          levelBuilt(false),
          state(PlayState::Playing),
          zoom(kDefaultZoom),
          pendingTeleport(false),
          pendingTeleportTimer(0.0f),
          pendingTeleportPlayerIndex(-1),
          pendingTeleportTarget{0,0},
          pendingTeleportTeleId(0)
    {
    }

    GameplayScene::~GameplayScene()
    {
        RestoreLighting();
        GM_MN.setMouseScrollCallback({});
    }

    void GameplayScene::ApplyNightLighting()
    {
        if (!lightingStateCaptured)
        {
            previousAmbientColor = LGT_MN.GetGlobalAmbientLightColor();
            previousAmbientDirection = LGT_MN.GetGlobalAmbientLightDirection();
            previousAmbientDiffuse = LGT_MN.GetGlobalAmbientLightDiffuse();
            previousAmbientSpecular = LGT_MN.GetGlobalAmbientLightSpecular();
            lightingStateCaptured = true;
        }

        SetPuzzleGameplayLightingEnabled(true);
        LGT_MN.SetGlobalAmbientLightColor(glm::vec3(0.05f, 0.06f, 0.10f));
        LGT_MN.SetGlobalAmbientLightDirection(glm::vec3(0.03f, 0.04f, 0.06f));
        LGT_MN.SetGlobalAmbientLightDiffuse(glm::vec3(0.17f, 0.18f, 0.24f));
        LGT_MN.SetGlobalAmbientLightSpecular(glm::vec3(0.10f, 0.10f, 0.12f));
    }

    void GameplayScene::RestoreLighting()
    {
        SetPuzzleGameplayLightingEnabled(false);
        if (!lightingStateCaptured) return;

        LGT_MN.SetGlobalAmbientLightColor(previousAmbientColor);
        LGT_MN.SetGlobalAmbientLightDirection(previousAmbientDirection);
        LGT_MN.SetGlobalAmbientLightDiffuse(previousAmbientDiffuse);
        LGT_MN.SetGlobalAmbientLightSpecular(previousAmbientSpecular);
        lightingStateCaptured = false;
    }

    void GameplayScene::loadResources()
    {
        RS_MN.LoadFont(PUZZLERPG_FONT_MAIN_KEY, PUZZLERPG_FONT_MAIN_PATH, PUZZLERPG_FONT_MAIN_SIZE);
        ResetPuzzleSpriteAssetCache();
        RegisterPuzzleSpriteAssets();
        AUD_MN.Load(PUZZLERPG_MUSIC_GAMEPLAY_KEY, PUZZLERPG_MUSIC_GAMEPLAY_PATH, CosmicEngine::SoundType::Music);
        // PuzzleRPG UI textures
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_KEY, PUZZLERPG_UI_BUTTON_IDLE_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_KEY, PUZZLERPG_UI_BUTTON_HOVER_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_M_KEY, PUZZLERPG_UI_BUTTON_IDLE_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_M_KEY, PUZZLERPG_UI_BUTTON_HOVER_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_L_KEY, PUZZLERPG_UI_BUTTON_IDLE_L_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_L_KEY, PUZZLERPG_UI_BUTTON_HOVER_L_PATH);
        // Inventory cell background
        RS_MN.LoadTexture(PUZZLERPG_UI_CELL_KEY, PUZZLERPG_UI_CELL_PATH);
        // Dialog message background
        RS_MN.LoadTexture(PUZZLERPG_UI_MESSAGE_KEY, PUZZLERPG_UI_MESSAGE_PATH);
        // Focus indicator sprite-sheet: 1 row x 4 columns.
        RS_MN.LoadTextureSheet(PUZZLERPG_UI_FOCUS_KEY, PUZZLERPG_UI_FOCUS_PATH, PUZZLERPG_UI_FOCUS_ROWS, PUZZLERPG_UI_FOCUS_COLS, PUZZLERPG_UI_FOCUS_PAD);
    }

    void GameplayScene::init()
    {
        RegisterPuzzleInputActions();
        StopMenuMusic();
        AUD_MN.Play(PUZZLERPG_MUSIC_GAMEPLAY_KEY, 1.0f, true, true);

        auto &session = GameSession::Get();

        runtime = std::make_unique<LevelRuntime>();
        dialog  = std::make_unique<DialogBox>("puzzle_font");
        ApplyNightLighting();

        // Ensure debug visuals/labels are off when entering gameplay.
        SetPuzzleDebugVisualsEnabled(false);
        SetPuzzleDebugLabelsEnabled(false);

        // Callback de fin de nivel (singleplayer y host).
        runtime->SetLevelCompletedCallback([this]() { HandleLevelCompleted(); });
        // Callback de teletransporte (solo el host/SP "arma" el viaje;
        // los clientes solo lo veran replicado mediante Msg_PlayerState).
        runtime->SetTeleporterStepCallback(
            [this](PuzzlePlayer &p, TeleporterObject &t) { OnTeleporterStep(p, t); });

        // El cliente espera el nivel desde el host. El host y singleplayer
        // cargan desde disco inmediatamente.
        if (session.mode == GameSession::NetworkMode::Client)
        {
            waitingForLevel = true;
        }
        else
        {
            if (!LoadLevelFromFile(session.selectedLevel, levelData))
            {
                levelData = MakeEmptyLevel("debug", 16, 16);
            }
            BuildLevelAndSpawn(0);
        }

        // HUD.
        hudLabel = new CosmicEngine::UIText("", "puzzle_font",
                                            glm::vec2(40.0f, 30.0f),
                                            glm::vec2(900.0f, 40.0f), true);
        hudLabel->SetTextColor(glm::vec3(0.95f, 0.95f, 0.95f));
        UI_MN.AddElement(hudLabel);
        // Hide HUD label entirely in gameplay (no textual hints requested)
        hudLabel->SetVisible(false);

        // Single-slot inventory displayed as a UI image (cell) with dynamic
        // overlay rendered on top in UpdateHud. Center horizontally and lift
        // it a bit from the bottom. Make it bigger for visibility.
        inventoryBaseSize = glm::vec2(96.0f, 96.0f);
        // Use the engine UI base size so the inventory shares the same
        // coordinate space as UIImage/UIText in other menus.
        glm::vec2 windowSize = CAM_MN.GetBaseWindowSize();
        inventoryBasePos = glm::vec2(windowSize.x * 0.5f - inventoryBaseSize.x * 0.5f,
                        windowSize.y - 140.0f - inventoryBaseSize.y * 0.5f);
        inventoryBaseImage = new CosmicEngine::UIImage("ui/cell", inventoryBasePos, inventoryBaseSize, true);
        UI_MN.AddElement(inventoryBaseImage);

        // Label under the inventory cell, centered horizontally.
        glm::vec2 labelPos = glm::vec2(inventoryBasePos.x + inventoryBaseSize.x * 0.5f - 100.0f,
                   inventoryBasePos.y + inventoryBaseSize.y + 6.0f);
        inventorySlotLabel = new CosmicEngine::UIText("Objeto actual", "puzzle_font", labelPos, glm::vec2(200.0f, 24.0f), true);
        inventorySlotLabel->SetTextColor(glm::vec3(0.95f, 0.95f, 0.95f));
        UI_MN.AddElement(inventorySlotLabel);

        // Overlay element drawn after the UIImage so the sprite appears on top.
        inventoryOverlay = new InventoryOverlayElement(inventoryBasePos, inventoryBaseSize, false);
        UI_MN.AddElement(inventoryOverlay);

        // Dialog message background + text (initially hidden)
        glm::vec2 messageSize = glm::vec2(768.0f, 256.0f);
        glm::vec2 messagePos = glm::vec2(windowSize.x * 0.5f - messageSize.x * 0.5f,
                         windowSize.y - messageSize.y - 30.0f);
        dialogMessageBg = new CosmicEngine::UIImage("ui/message", messagePos, messageSize, false);
        UI_MN.AddElement(dialogMessageBg);
        dialogMessageText = new CosmicEngine::UIText("", "puzzle_font", messagePos, messageSize, false);
        dialogMessageText->SetTextColor(glm::vec3(0.0f, 0.0f, 0.0f));
        UI_MN.AddElement(dialogMessageText);

        BuildOverlays();

        GM_MN.setMouseScrollCallback([this](double, double yoffset)
        {
            zoom = ClampZoom(zoom + static_cast<float>(yoffset) * kZoomStep);
            CAM_MN.SetZoom(zoom);
        });

        if (session.mode != GameSession::NetworkMode::SinglePlayer)
        {
            SetupNetwork();
        }

        // Zoom inicial: acercar la accion.
        CAM_MN.SetZoom(zoom);
        UpdateCameraFollow();
        UpdateHud();

        
    }

    void GameplayScene::BuildLevelAndSpawn(int localPlayerIndex)
    {
        if (levelBuilt) return;
        runtime->Build(levelData);

        std::vector<const LevelObject *> spawns;
        for (const auto &object : levelData.objects)
        {
            if (object.classId == Class_PlayerSpawn) spawns.push_back(&object);
        }
        std::sort(spawns.begin(), spawns.end(),
            [](const LevelObject *left, const LevelObject *right)
            {
                return left->id < right->id;
            });

        Cell spawn0 = spawns.empty()    ? Cell{1, 1} : Cell{spawns[0]->x, spawns[0]->y};
        Cell spawn1 = spawns.size() < 2 ? Cell{2, 1} : Cell{spawns[1]->x, spawns[1]->y};
        int  spawn0DrawLayer = spawns.empty()    ? DefaultDrawLayerForClass(Class_PlayerSpawn)
                             : spawns[0]->drawLayer;
        int  spawn1DrawLayer = spawns.size() < 2 ? DefaultDrawLayerForClass(Class_PlayerSpawn)
                             : spawns[1]->drawLayer;

        auto &session = GameSession::Get();
        bool multiplayer = (session.mode != GameSession::NetworkMode::SinglePlayer);

        Cell localSpawn  = (localPlayerIndex == 0) ? spawn0 : spawn1;
        int  localDrawLayer = (localPlayerIndex == 0) ? spawn0DrawLayer : spawn1DrawLayer;
        localPlayer = new PuzzlePlayer(localPlayerIndex, localSpawn, true, session.username,
                                       localDrawLayer, multiplayer);
        OBJ_MN.Add(localPlayer);
        runtime->AddPlayer(localPlayer);

        if (multiplayer)
        {
            int  remoteIndex = (localPlayerIndex == 0) ? 1 : 0;
            Cell remoteSpawn = (remoteIndex == 0)      ? spawn0 : spawn1;
            int  remoteDrawLayer = (remoteIndex == 0) ? spawn0DrawLayer : spawn1DrawLayer;
            // Nombre temporal hasta que llegue Msg_PlayerJoin / hostUsername.
            remotePlayer = new PuzzlePlayer(remoteIndex, remoteSpawn, false, std::string("Conectando..."),
                                            remoteDrawLayer, true);
            OBJ_MN.Add(remotePlayer);
            runtime->AddPlayer(remotePlayer);
        }

        // Set correct Y-sorted layers immediately so the first frame is correct.
        UpdateYSortLayers();

        levelBuilt = true;
        waitingForLevel = false;

        CAM_MN.SetFocusObject(localPlayer, kTileSize * 0.5f, kTileSize * 0.5f);
    }

    void GameplayScene::update(double deltaTime)
    {
        auto clearFocusIndicator = [this]()
        {
            focusAnimTimer = 0.0f;
            hasFocusedFrontCell = false;
            if (focusIndicator)
            {
                OBJ_MN.Remove(focusIndicator->GetID());
                focusIndicator = nullptr;
            }
        };

        if (GameSession::Get().mode != GameSession::NetworkMode::SinglePlayer)
        {
            NET_MN.ProcessMessages();
        }

        // Teletransporte programado (host/SP): tras el retardo aplicamos el salto.
        if (pendingTeleport)
        {
            pendingTeleportTimer -= static_cast<float>(deltaTime);
            if (pendingTeleportTimer <= 0.0f)
            {
                pendingTeleport = false;
                PuzzlePlayer *p = nullptr;
                if (localPlayer  && localPlayer->GetPlayerIndex()  == pendingTeleportPlayerIndex) p = localPlayer;
                else if (remotePlayer && remotePlayer->GetPlayerIndex() == pendingTeleportPlayerIndex) p = remotePlayer;
                if (p)
                {
                    p->TeleportToCell(pendingTeleportTarget);
                    p->SetLastTeleporterId(pendingTeleportTeleId);
                    runtime->RebuildPlateStates();
                    if (networkSetup && NET_MN.IsServer()) SendPlayerState(*p);
                }
            }
        }

        // Vigilancia de conexion (solo cliente; el host puede no tener peers).
        if (networkSetup && NET_MN.IsClient() &&
            state != PlayState::ConnectionLost && !NET_MN.IsConnected())
        {
            SetState(PlayState::ConnectionLost);
        }

        // Detect whether the local player just finished a move this frame
        // (was moving last frame but no longer). If so, check for any held
        // movement input and immediately turn/move accordingly to keep
        // movement fluid.
        if (localPlayer)
        {
            bool nowMoving = localPlayer->IsMoving();

            // Update pending facing-change timer (if any).
            if (pendingFacingChange)
            {
                pendingFacingTimer -= static_cast<float>(deltaTime);

                // If key was released during the grace window, cancel pending move.
                if (pendingFacingAction && !INP_MN.IsActionPressed(pendingFacingAction, CosmicEngine::KeyRelease))
                {
                    pendingFacingChange = false;
                    pendingFacingAction = nullptr;
                    pendingFacingDir = -1;
                }
                else if (pendingFacingTimer <= 0.0f)
                {
                    // Grace window expired and key still held: perform the move.
                    int facing = pendingFacingDir;
                    if (!networkSetup || !NET_MN.IsClient())
                    {
                        if (TryFaceThenMove(*runtime, *localPlayer, facing))
                        {
                            runtime->HandlePlayerArrived(*localPlayer);
                            if (networkSetup && NET_MN.IsServer()) SendPlayerState(*localPlayer);
                        }
                    }
                    else
                    {
                        if (!pendingNetMove)
                        {
                            CosmicEngine::NetworkMessage msg(
                                Net::Msg_PlayerIntent,
                                {
                                    {"intent", Net::Intent_Move},
                                    {"dir", facing}
                                });
                            NET_MN.SendToServer(msg);
                            pendingNetMove = true;
                        }
                    }

                    pendingFacingChange = false;
                    pendingFacingAction = nullptr;
                    pendingFacingDir = -1;
                }
            }
            if (prevLocalPlayerWasMoving && !nowMoving)
            {
                bool skipQueued = false;
                // If a teleport is pending for this player, do not queue new moves;
                // the player must wait until the teleport completes.
                if (pendingTeleport && pendingTeleportPlayerIndex == localPlayer->GetPlayerIndex())
                {
                    skipQueued = true;
                }

                if (!skipQueued)
                {
                    // Check held movement inputs (prefer order: Up, Down, Left, Right)
                    const std::pair<const char *, int> moveActions[] = {
                        { kActionMoveUp, Net::Face_Up },
                        { kActionMoveDown, Net::Face_Down },
                        { kActionMoveLeft, Net::Face_Left },
                        { kActionMoveRight, Net::Face_Right }
                    };

                    for (const auto &[actionName, facing] : moveActions)
                    {
                        if (INP_MN.IsActionPressed(actionName, CosmicEngine::KeyRelease))
                        {
                            // If facing differs, update facing. Start a small
                            // grace timer (0.25s) so that a quick tap only changes
                            // facing without initiating movement. If the key is
                            // kept pressed beyond the timer, the move will occur.
                            if (localPlayer->GetFacing() != facing)
                            {
                                localPlayer->SetFacing(facing);
                                pendingFacingChange = true;
                                pendingFacingTimer = 0.25f;
                                pendingFacingDir = facing;
                                pendingFacingAction = actionName;
                                if (networkSetup && NET_MN.IsClient())
                                {
                                    NET_MN.SendToServer(CosmicEngine::NetworkMessage(
                                        Net::Msg_PlayerIntent,
                                        {{"intent", Net::Intent_Move}, {"dir", facing}}));
                                }
                            }
                            else
                            {
                                // Facing already matches: proceed to move immediately.
                                if (!networkSetup || !NET_MN.IsClient())
                                {
                                    if (TryFaceThenMove(*runtime, *localPlayer, facing))
                                    {
                                        runtime->HandlePlayerArrived(*localPlayer);
                                        if (networkSetup && NET_MN.IsServer()) SendPlayerState(*localPlayer);
                                    }
                                }
                                else
                                {
                                    if (!pendingNetMove)
                                    {
                                        CosmicEngine::NetworkMessage msg(
                                            Net::Msg_PlayerIntent,
                                            {
                                                {"intent", Net::Intent_Move},
                                                {"dir", facing}
                                            });
                                        NET_MN.SendToServer(msg);
                                        pendingNetMove = true;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
            // update prev flag for next frame
            prevLocalPlayerWasMoving = nowMoving;
        }

        // ESC abre/cierra el menu de pausa. No cierra la conexion.
        if (INP_MN.IsActionPressed(kActionPause, CosmicEngine::KeyDown))
        {
            if (state == PlayState::Playing)      SetState(PlayState::Paused);
            else if (state == PlayState::Paused)  SetState(PlayState::Playing);
        }

        // F3 alterna los "bodies/primitivas" de debug (placeholders y texto
        // encima de tiles y objetos). Los sprites reales no se ven afectados.
        // Disabled during gameplay (Playing state) to avoid showing debug info
        // while the game is being played.
        if (INP_MN.IsKeyPressed(GLFW_KEY_F3, CosmicEngine::KeyDown))
        {
            if (state != PlayState::Playing)
            {
                TogglePuzzleDebugVisuals();
                SetPuzzleDebugLabelsEnabled(IsPuzzleDebugVisualsEnabled());
            }
        }

        if (state == PlayState::Playing)
        {
            if (!waitingForLevel && localPlayer)
            {
                ApplyInput();
            }
        }

        UpdateCameraFollow();
        UpdateHud();
        UpdateYSortLayers();

        // Focus indicator: detect if there's an interactable/pickup/button/object
        // in front of the local player and drive the small animation state.
        if (localPlayer && runtime)
        {
            Cell front = CellInFront(localPlayer->GetCell(), localPlayer->GetFacing());
            if (!runtime->IsCellInsideLevel(front))
            {
                // Out of bounds -> no focus.
                clearFocusIndicator();
            }
            else
            {
                bool hasInteractable = false;
                auto *pickup = runtime->GetPickupAtCell(front);
                if (pickup != nullptr && !pickup->IsCarried()) hasInteractable = true;
                if (runtime->GetButtonAtCell(front) != nullptr) hasInteractable = true;
                if (runtime->GetDoorAtCell(front) != nullptr) hasInteractable = true;
                if (runtime->GetPushableAtCell(front) != nullptr) hasInteractable = true;
                if (runtime->GetPlateAtCell(front) != nullptr) hasInteractable = true;
                if (runtime->GetSignAtCell(front) != nullptr) hasInteractable = true;

                if (hasInteractable)
                {
                    if (!hasFocusedFrontCell || focusedFrontCell != front)
                    {
                        focusAnimTimer = 0.0f;
                        focusedFrontCell = front;
                        hasFocusedFrontCell = true;
                    }

                    focusAnimTimer += static_cast<float>(deltaTime);
                    if (!focusIndicator)
                    {
                        focusIndicator = new FocusIndicatorObject(front);
                        OBJ_MN.Add(focusIndicator);
                    }

                    FocusIndicatorObject *fi = static_cast<FocusIndicatorObject *>(focusIndicator);
                    fi->SetCell(front);
                    const float cycle = 2.3f;
                    float t = std::fmod(focusAnimTimer, cycle);
                    int frame = 0;
                    if (t < 2.0f) frame = 0;
                    else if (t < 2.1f) frame = 3;
                    else if (t < 2.2f) frame = 2;
                    else frame = 1;
                    fi->SetFrame(frame);
                }
                else
                {
                    clearFocusIndicator();
                }
            }
        }
        else
        {
            clearFocusIndicator();
        }
    }

    void GameplayScene::UpdateYSortLayers()
    {
        if (!runtime) return;
        bool anyChanged = false;

        for (PuzzlePlayer *p : runtime->GetPlayers())
        {
            if (!p) continue;
            short newLayer = static_cast<short>(2 + p->GetCell().y + 1);
            if (p->GetLayerId() != newLayer) { p->SetLayerId(newLayer); anyChanged = true; }
        }

        for (PushableObject *pb : runtime->GetPushables())
        {
            if (!pb) continue;
            short newLayer = static_cast<short>(2 + pb->GetCell().y + 1);
            if (pb->GetLayerId() != newLayer) { pb->SetLayerId(newLayer); anyChanged = true; }
        }

        if (anyChanged) OBJ_MN.SortByLayer();
    }

    void GameplayScene::UpdateCameraFollow()
    {
        // SetFocusObject toma una instantanea; hay que llamarlo cada frame
        // para que la camara siga al jugador mientras se mueve.
        if (localPlayer)
        {
            CAM_MN.SetFocusObject(localPlayer, kTileSize * 0.5f, kTileSize * 0.5f);
        }
    }

    void GameplayScene::draw()
    {
        RS_MN.RenderRectangle(glm::vec2(-4096.0f, -4096.0f), glm::vec2(8192.0f, 8192.0f),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              glm::vec3(levelData.backgroundColor.r / 255.0f,
                                        levelData.backgroundColor.g / 255.0f,
                                        levelData.backgroundColor.b / 255.0f),
                              1.0f, 1.0f, true);

        if (waitingForLevel)
        {
            RS_MN.RenderText("Esperando nivel del host...", "puzzle_font",
                             glm::vec3(700.0f, 500.0f, 0.0f),
                             glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                             glm::vec3(1.0f, 0.95f, 0.5f), 1.0f);
        }
    }

    void GameplayScene::reset()
    {
        RestoreLighting();
    }

    void GameplayScene::ApplyInput()
    {
        if (dialog && dialog->IsVisible())
        {
            if (INP_MN.IsActionPressed(kActionInteract, CosmicEngine::KeyDown))
            {
                dialog->DismissIfInfo();
            }
            return;
        }

        if (localPlayer->IsMoving()) return;
        if (localPlayer->IsFrozen()) return;

        const std::pair<const char *, int> moveActions[] = {
            { kActionMoveUp, Net::Face_Up },
            { kActionMoveDown, Net::Face_Down },
            { kActionMoveLeft, Net::Face_Left },
            { kActionMoveRight, Net::Face_Right }
        };

        for (const auto &[actionName, facing] : moveActions)
        {
            // Trigger on press. Additionally, if the action is held and the
            // player is already facing the same direction, allow repeating
            // the move while the input is held (KeyRelease).
            const bool pressed = INP_MN.IsActionPressed(actionName, CosmicEngine::KeyDown);
            const bool held = INP_MN.IsActionPressed(actionName, CosmicEngine::KeyRelease);
            if (!pressed && !(held && localPlayer->GetFacing() == facing))
            {
                continue;
            }

            // If the direction changes, only rotate first and open a small
            // grace window. A quick release inside that window should not
            // start movement.
            if (localPlayer->GetFacing() != facing)
            {
                localPlayer->SetFacing(facing);
                pendingFacingChange = true;
                pendingFacingTimer = 0.5f;
                pendingFacingDir = facing;
                pendingFacingAction = actionName;

                if (networkSetup && NET_MN.IsServer())
                    SendPlayerState(*localPlayer);
                else if (networkSetup && NET_MN.IsClient())
                {
                    // Sync facing to server so TryInteractInFront uses the
                    // correct direction when the player interacts next.
                    NET_MN.SendToServer(CosmicEngine::NetworkMessage(
                        Net::Msg_PlayerIntent,
                        {{"intent", Net::Intent_Move}, {"dir", facing}}));
                }
                return;
            }

            // If we are still inside the grace window for this facing change,
            // movement must wait for the timer logic in update().
            if (pendingFacingChange && pendingFacingDir == facing && pendingFacingAction == actionName)
            {
                return;
            }

            pendingFacingChange = false;
            pendingFacingTimer = 0.0f;
            pendingFacingDir = -1;
            pendingFacingAction = nullptr;

            if (networkSetup && NET_MN.IsClient())
            {
                if (!pendingNetMove)
                {
                    CosmicEngine::NetworkMessage msg(
                        Net::Msg_PlayerIntent,
                        {
                            {"intent", Net::Intent_Move},
                            {"dir", facing}
                        });
                    NET_MN.SendToServer(msg);
                    pendingNetMove = true;
                }
                return;
            }

            if (TryFaceThenMove(*runtime, *localPlayer, facing))
            {
                runtime->HandlePlayerArrived(*localPlayer);
            }
            if (networkSetup && NET_MN.IsServer()) SendPlayerState(*localPlayer);
            return;
        }

        const int slotKeys[4] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4 };
        for (int i = 0; i < 4; ++i)
        {
            if (INP_MN.IsKeyPressed(slotKeys[i], CosmicEngine::KeyDown))
            {
                localPlayer->SetSelectedSlot(i);
            }
        }

        if (INP_MN.IsActionPressed(kActionInteract, CosmicEngine::KeyDown))
        {
            auto tryInventoryInteraction = [this]()
            {
                int sel = localPlayer->GetSelectedSlot();
                if (localPlayer->PeekInventoryAt(sel) == 0)
                {
                    return runtime->TryPickupInFront(*localPlayer);
                }
                return runtime->TryPlaceInFront(*localPlayer);
            };

            if (networkSetup && NET_MN.IsClient())
            {
                NET_MN.SendToServer(CosmicEngine::NetworkMessage(
                    Net::Msg_PlayerIntent,
                    {{"intent", Net::Intent_Interact}}));
                return;
            }
            if (!runtime->TryInteractInFront(*localPlayer))
            {
                std::string text = runtime->GetDialogInFront(*localPlayer);
                if (!text.empty() && dialog)
                {
                    dialog->ShowInfo(text, [](){});
                }
                else
                {
                    tryInventoryInteraction();
                }
            }
            runtime->RebuildPlateStates();
            if (networkSetup && NET_MN.IsServer()) SendPlayerState(*localPlayer);
        }
    }

    // ──────────────────────────────────────────────────────────────────
    // Red
    // ──────────────────────────────────────────────────────────────────

    void GameplayScene::SetupNetwork()
    {
        NET_MN.ClearAllHandlers();

        if (NET_MN.IsServer())
        {
            // Cuando un cliente se conecta, le mandamos el nivel actual.
            NET_MN.RegisterHandler(CosmicEngine::NetMessageType::ClientConnect,
                [this](int senderId, const nlohmann::json &payload)
                {
                    (void)senderId;
                    int clientId = payload.value("clientId", -1);
                    if (clientId < 0) return;

                    nlohmann::json levelJson = SerializeLevel(levelData);
                    CosmicEngine::NetworkMessage msg(
                        Net::Msg_LevelLoad,
                        {
                            {"levelName",     levelData.name},
                            {"level",         levelJson},
                            {"assignedIndex", 1},
                            {"hostUsername",  GameSession::Get().username}
                        });
                    NET_MN.SendToClient(clientId, msg);

                    if (localPlayer)
                    {
                        SendPlayerState(*localPlayer);
                    }
                });

            NET_MN.RegisterHandler(Net::Msg_PlayerIntent,
                [this](int senderId, const nlohmann::json &payload)
                {
                    (void)senderId;
                    if (!remotePlayer) return;
                    int intent = payload.value("intent", 0);
                    if (intent == Net::Intent_Move)
                    {
                        int dir = payload.value("dir", remotePlayer->GetFacing());
                        if (!remotePlayer->IsFrozen())
                        {
                            if (TryFaceThenMove(*runtime, *remotePlayer, dir))
                            {
                                runtime->HandlePlayerArrived(*remotePlayer);
                            }
                        }
                        else
                        {
                            remotePlayer->SetFacing(dir);
                        }
                        SendPlayerState(*remotePlayer);
                    }
                    else if (intent == Net::Intent_Interact)
                    {
                        if (!runtime->TryInteractInFront(*remotePlayer))
                        {
                            std::string text = runtime->GetDialogInFront(*remotePlayer);
                            if (!text.empty())
                            {
                                CosmicEngine::NetworkMessage dlg(
                                    Net::Msg_Dialog,
                                    {{"text", text}, {"playerIndex", remotePlayer->GetPlayerIndex()}});
                                NET_MN.SendToClient(senderId, dlg);
                            }
                            else
                            {
                                int sel = remotePlayer->GetSelectedSlot();
                                if (remotePlayer->PeekInventoryAt(sel) == 0)
                                {
                                    runtime->TryPickupInFront(*remotePlayer);
                                }
                                else
                                {
                                    runtime->TryPlaceInFront(*remotePlayer);
                                }
                            }
                        }
                        runtime->RebuildPlateStates();
                        SendPlayerState(*remotePlayer);
                    }
                    else if (intent == Net::Intent_Pickup)
                    {
                        runtime->TryPickupInFront(*remotePlayer);
                        runtime->RebuildPlateStates();
                        SendPlayerState(*remotePlayer);
                    }
                    else if (intent == Net::Intent_Place)
                    {
                        runtime->TryPlaceInFront(*remotePlayer);
                        runtime->RebuildPlateStates();
                        SendPlayerState(*remotePlayer);
                    }
                });

            // Recibimos el nombre del jugador remoto.
            NET_MN.RegisterHandler(Net::Msg_PlayerJoin,
                [this](int, const nlohmann::json &payload)
                {
                    if (!remotePlayer) return;
                    std::string username = payload.value("username", std::string("Jugador 2"));
                    remotePlayer->SetUsername(username);
                });

            // El host no cierra la partida cuando se va un cliente.
            NET_MN.RegisterHandler(CosmicEngine::NetMessageType::ClientDisconnect,
                [](int, const nlohmann::json &) {});
        }
        else if (NET_MN.IsClient())
        {
            // El servidor nos envia el nivel; lo cargamos y comenzamos.
            NET_MN.RegisterHandler(
                Net::Msg_LevelLoad,
                [this](int, const nlohmann::json &payload)
                {
                    if (levelBuilt) return;
                    std::string levelName = payload.value("levelName", std::string("net"));
                    int assignedIndex     = payload.value("assignedIndex", 1);

                    nlohmann::json levelJson = payload.value("level", nlohmann::json::object());
                    LevelData incoming;
                    if (!DeserializeLevel(levelJson, incoming))
                    {
                        incoming = MakeEmptyLevel(levelName, 16, 16);
                    }
                    levelData = incoming;
                    GameSession::Get().selectedLevel = levelData.name;

                    BuildLevelAndSpawn(assignedIndex);

                    // Establece nombre del host y manda el nuestro al servidor.
                    if (remotePlayer)
                    {
                        std::string hostName = payload.value("hostUsername", std::string("Host"));
                        remotePlayer->SetUsername(hostName);
                    }
                    NET_MN.SendToServer(CosmicEngine::NetworkMessage(
                        Net::Msg_PlayerJoin,
                        {{"username", GameSession::Get().username}}));
                });

            NET_MN.RegisterHandler(Net::Msg_PlayerState,
                [this](int, const nlohmann::json &payload)
                {
                    int playerIndex = payload.value("playerIndex", -1);
                    int facing      = payload.value("facing", Net::Face_Down);
                    int x           = payload.value("x", 0);
                    int y           = payload.value("y", 0);
                    if (playerIndex < 0)
                    {
                        return;
                    }

                    // Always release the move-pending lock when the server
                    // acknowledges the local player's state, even if the cell
                    // hasn't changed (e.g. blocked move during teleport delay).
                    if (localPlayer && localPlayer->GetPlayerIndex() == playerIndex)
                        pendingNetMove = false;

                    ApplyReplicatedPlayerState(playerIndex, facing, Cell{x, y});
                });

            // Snapshot del estado del mundo enviado por el host.
            NET_MN.RegisterHandler(Net::Msg_WorldState,
                [this](int, const nlohmann::json &payload)
                {
                    ApplyWorldState(payload);
                });

            // El host nos dice que el nivel termino.
            NET_MN.RegisterHandler(Net::Msg_GameEnd,
                [this](int, const nlohmann::json &)
                {
                    if (state != PlayState::Completed) SetState(PlayState::Completed);
                });

            // Dialogo enviado desde el host (por ejemplo, tras un Interact remoto).
            NET_MN.RegisterHandler(Net::Msg_Dialog,
                [this](int, const nlohmann::json &payload)
                {
                    std::string text = payload.value("text", std::string());
                    if (!text.empty() && dialog) dialog->ShowInfo(text, [](){});
                });

            // Si el host se cae, ClientDisconnect llega con el id local del cliente.
            NET_MN.RegisterHandler(CosmicEngine::NetMessageType::ClientDisconnect,
                [this](int, const nlohmann::json &payload)
                {
                    int clientId = payload.value("clientId", -1);
                    if (clientId == NET_MN.GetLocalClientId())
                    {
                        SetState(PlayState::ConnectionLost);
                    }
                });
        }

        networkSetup = true;
    }

    void GameplayScene::TeardownNetwork()
    {
        if (!networkSetup) return;
        NET_MN.ClearAllHandlers();
        if (NET_MN.IsConnected())
        {
            NET_MN.Disconnect();
        }
        networkSetup = false;
    }

    // ──────────────────────────────────────────────────────────────────
    // Overlays (pausa / conexion perdida)
    // ──────────────────────────────────────────────────────────────────

    void GameplayScene::BuildOverlays()
    {
        const glm::vec2 panelPos(640.0f, 330.0f);
        const glm::vec2 buttonSize(420.0f, 70.0f);

        // ── PAUSA ──
        auto *pauseTitle = new CosmicEngine::UIText("PAUSA", "puzzle_font",
                                                    panelPos + glm::vec2(170.0f, 20.0f),
                                                    glm::vec2(300.0f, 90.0f), false);
        pauseTitle->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
        pauseTitle->SetTextScale(2.0f);
        UI_MN.AddElement(pauseTitle);
        pauseOverlay.push_back(pauseTitle);

        {
            std::string label = "Reanudar";
            auto choose_idle = [](const std::string &s) {
                size_t l = s.size();
                if (l <= 4) return std::string("ui/button-idle");
                if (l <= 16) return std::string("ui/button-idle-m");
                return std::string("ui/button-idle-l");
            };
            std::string idleKey = choose_idle(label);
            std::string hoverKey = idleKey; auto ph = hoverKey.find("idle"); if (ph != std::string::npos) hoverKey.replace(ph, 4, "hover");
            auto *resumeBtn = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                         panelPos + glm::vec2(110.0f, 140.0f),
                                                         buttonSize, true, false);
            resumeBtn->SetHoverTexture(hoverKey);
            resumeBtn->SetTextureScale(1.06f);
            resumeBtn->SetTextOffset(glm::vec2(0.0f, -4.0f));
            resumeBtn->SetTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
            resumeBtn->SetSelectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f));
            resumeBtn->SetOnClick([this]() { SetState(PlayState::Playing); });
            UI_MN.AddElement(resumeBtn);
            pauseOverlay.push_back(resumeBtn);
        }

        {
            std::string label = "Salir al menu principal";
            auto choose_idle = [](const std::string &s) {
                size_t l = s.size();
                if (l <= 4) return std::string("ui/button-idle");
                if (l <= 16) return std::string("ui/button-idle-m");
                return std::string("ui/button-idle-l");
            };
            std::string idleKey = choose_idle(label);
            std::string hoverKey = idleKey; auto ph = hoverKey.find("idle"); if (ph != std::string::npos) hoverKey.replace(ph, 4, "hover");
            auto *exitBtn = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                       panelPos + glm::vec2(110.0f, 230.0f),
                                                       buttonSize, true, false);
            exitBtn->SetHoverTexture(hoverKey);
            exitBtn->SetTextureScale(1.06f);
            exitBtn->SetTextOffset(glm::vec2(0.0f, -4.0f));
            exitBtn->SetTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
            exitBtn->SetSelectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f));
            exitBtn->SetOnClick([this]() { GoToMainMenu(); });
            UI_MN.AddElement(exitBtn);
            pauseOverlay.push_back(exitBtn);
        }

        // ── CONEXION PERDIDA ──
        auto *lostTitle = new CosmicEngine::UIText("Conexion perdida", "puzzle_font",
                                                   panelPos + glm::vec2(80.0f, 30.0f),
                                                   glm::vec2(500.0f, 60.0f), false);
        lostTitle->SetTextColor(glm::vec3(0.95f, 0.35f, 0.35f));
        UI_MN.AddElement(lostTitle);
        lostOverlay.push_back(lostTitle);

        auto *lostDesc = new CosmicEngine::UIText("Se ha perdido la conexion con el host.",
                                                  "puzzle_font",
                                                  panelPos + glm::vec2(40.0f, 110.0f),
                                                  glm::vec2(560.0f, 50.0f), false);
        lostDesc->SetTextColor(glm::vec3(0.9f, 0.9f, 0.9f));
        UI_MN.AddElement(lostDesc);
        lostOverlay.push_back(lostDesc);

        {
            std::string label = "Volver al menu principal";
            auto choose_idle = [](const std::string &s) {
                size_t l = s.size();
                if (l <= 4) return std::string("ui/button-idle");
                if (l <= 16) return std::string("ui/button-idle-m");
                return std::string("ui/button-idle-l");
            };
            std::string idleKey = choose_idle(label);
            std::string hoverKey = idleKey; auto ph = hoverKey.find("idle"); if (ph != std::string::npos) hoverKey.replace(ph, 4, "hover");
            auto *lostBtn = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                       panelPos + glm::vec2(110.0f, 230.0f),
                                                       buttonSize, true, false);
            lostBtn->SetHoverTexture(hoverKey);
            lostBtn->SetTextureScale(1.06f);
            lostBtn->SetTextOffset(glm::vec2(0.0f, -4.0f));
            lostBtn->SetOnClick([this]() { GoToMainMenu(); });
            UI_MN.AddElement(lostBtn);
            lostOverlay.push_back(lostBtn);
        }

        // ── NIVEL COMPLETADO ──
        auto *winTitle = new CosmicEngine::UIText("!Nivel completado!", "puzzle_font",
                                                  panelPos + glm::vec2(10.0f, 20.0f),
                                                  glm::vec2(600.0f, 90.0f), false);
        winTitle->SetTextColor(glm::vec3(0.45f, 0.95f, 0.55f));
        winTitle->SetTextScale(2.0f);
        UI_MN.AddElement(winTitle);
        winOverlay.push_back(winTitle);

        auto *winDesc = new CosmicEngine::UIText("Tus ofrendas fueron recibidas... probablemente",
                                                 "puzzle_font",
                                                 panelPos + glm::vec2(20.0f, 110.0f),
                                                 glm::vec2(600.0f, 50.0f), false);
        winDesc->SetTextColor(glm::vec3(0.9f, 0.9f, 0.9f));
        UI_MN.AddElement(winDesc);
        winOverlay.push_back(winDesc);

        {
            std::string label = "Volver al menu principal";
            auto choose_idle = [](const std::string &s) {
                size_t l = s.size();
                if (l <= 4) return std::string("ui/button-idle");
                if (l <= 16) return std::string("ui/button-idle-m");
                return std::string("ui/button-idle-l");
            };
            std::string idleKey = choose_idle(label);
            std::string hoverKey = idleKey; auto ph = hoverKey.find("idle"); if (ph != std::string::npos) hoverKey.replace(ph, 4, "hover");
            auto *winBtn = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                      panelPos + glm::vec2(110.0f, 230.0f),
                                                      buttonSize, true, false);
            winBtn->SetHoverTexture(hoverKey);
            winBtn->SetTextureScale(1.06f);
            winBtn->SetTextOffset(glm::vec2(0.0f, -4.0f));
            winBtn->SetTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
            winBtn->SetSelectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f));
            winBtn->SetOnClick([this]() { GoToMainMenu(); });
            UI_MN.AddElement(winBtn);
            winOverlay.push_back(winBtn);
        }
    }

    void GameplayScene::SetState(PlayState newState)
    {
        state = newState;
        bool showPause = (state == PlayState::Paused);
        bool showLost  = (state == PlayState::ConnectionLost);
        bool showWin   = (state == PlayState::Completed);
        for (auto *e : pauseOverlay) e->SetVisible(showPause);
        for (auto *e : lostOverlay)  e->SetVisible(showLost);
        for (auto *e : winOverlay)   e->SetVisible(showWin);
    }

    void GameplayScene::GoToMainMenu()
    {
        GM_MN.setMouseScrollCallback({});
        AUD_MN.Stop(PUZZLERPG_MUSIC_GAMEPLAY_KEY);
        runtime.reset();
        if (focusIndicator)
        {
            OBJ_MN.Remove(focusIndicator->GetID());
            focusIndicator = nullptr;
        }
        localPlayer = nullptr;
        remotePlayer = nullptr;
        TeardownNetwork();
        GameSession::Get().mode = GameSession::NetworkMode::SinglePlayer;
        SCN_MN.ReplaceScene(new MainMenuScene());
    }

    void GameplayScene::SendPlayerState(const PuzzlePlayer &player)
    {
        if (!networkSetup || !NET_MN.IsServer())
        {
            return;
        }

        Cell cell = player.GetCell();
        CosmicEngine::NetworkMessage msg(
            Net::Msg_PlayerState,
            {
                {"playerIndex", player.GetPlayerIndex()},
                {"facing",      player.GetFacing()},
                {"x",           cell.x},
                {"y",           cell.y}
            });
        NET_MN.Broadcast(msg);

        // Replica tambien el estado del mundo (pickups, pushables, puertas, etc.)
        // para que cada accion del host se refleje en todos los clientes.
        BroadcastWorldState();
    }

    void GameplayScene::BroadcastWorldState()
    {
        if (!networkSetup || !NET_MN.IsServer() || !runtime)
        {
            return;
        }

        nlohmann::json pickupsJson = nlohmann::json::array();
        for (auto *p : runtime->GetPickups())
        {
            if (!p) continue;
            Cell c = p->GetCell();
            pickupsJson.push_back({{"id", p->GetObjectId()},
                                   {"x", c.x}, {"y", c.y},
                                   {"carried", p->IsCarried()},
                                   {"tex", p->GetTextureId()}});
        }
        nlohmann::json pushJson = nlohmann::json::array();
        for (auto *p : runtime->GetPushables())
        {
            if (!p) continue;
            Cell c = p->GetCell();
            pushJson.push_back({{"id", p->GetObjectId()}, {"x", c.x}, {"y", c.y}});
        }
        nlohmann::json buttonsJson = nlohmann::json::array();
        for (auto *b : runtime->GetButtons())
        {
            if (!b) continue;
            buttonsJson.push_back({{"id", b->GetObjectId()}, {"active", b->IsActive()}});
        }
        nlohmann::json doorsJson = nlohmann::json::array();
        for (auto *d : runtime->GetDoors())
        {
            if (!d) continue;
            doorsJson.push_back({{"id", d->GetObjectId()}, {"open", d->IsOpen()}});
        }
        nlohmann::json platesJson = nlohmann::json::array();
        for (auto *pl : runtime->GetPlates())
        {
            if (!pl) continue;
            platesJson.push_back({{"id", pl->GetObjectId()}, {"active", pl->IsActive()}});
        }
        nlohmann::json playersJson = nlohmann::json::array();
        for (auto *pp : runtime->GetPlayers())
        {
            if (!pp) continue;
            const auto &inv = pp->GetInventory();
            const auto &invIds = pp->GetInventoryObjectIds();
            nlohmann::json invJson = nlohmann::json::array();
            nlohmann::json invIdsJson = nlohmann::json::array();
            for (int k = 0; k < PuzzlePlayer::kInventorySize; ++k)
            {
                invJson.push_back(inv[k]);
                invIdsJson.push_back(invIds[k]);
            }
            playersJson.push_back({{"idx", pp->GetPlayerIndex()}, {"inv", invJson}, {"invIds", invIdsJson}});
        }

        CosmicEngine::NetworkMessage msg(
            Net::Msg_WorldState,
            {
                {"pickups",   pickupsJson},
                {"pushables", pushJson},
                {"buttons",   buttonsJson},
                {"doors",     doorsJson},
                {"plates",    platesJson},
                {"players",   playersJson}
            });
        NET_MN.Broadcast(msg);
    }

    void GameplayScene::ApplyWorldState(const nlohmann::json &payload)
    {
        if (!runtime) return;

        auto findById = [](auto &vec, int id) -> typename std::remove_reference<decltype(vec)>::type::value_type
        {
            for (auto *e : vec) if (e && e->GetObjectId() == id) return e;
            return nullptr;
        };

        if (payload.contains("pickups"))
        {
            for (const auto &j : payload["pickups"])
            {
                int id = j.value("id", -1);
                auto *p = findById(const_cast<std::vector<PickupObject *> &>(runtime->GetPickups()), id);
                if (!p) continue;
                p->SetCell(Cell{j.value("x", 0), j.value("y", 0)});
                p->SetCarried(j.value("carried", false));
            }
        }
        if (payload.contains("pushables"))
        {
            for (const auto &j : payload["pushables"])
            {
                int id = j.value("id", -1);
                auto *p = findById(const_cast<std::vector<PushableObject *> &>(runtime->GetPushables()), id);
                if (!p) continue;
                Cell target{j.value("x", 0), j.value("y", 0)};
                Cell current = p->GetCell();
                int dx = target.x - current.x;
                int dy = target.y - current.y;
                bool isAdjacentStep = (std::abs(dx) + std::abs(dy) == 1);
                if (target == current)
                {
                    continue;
                }
                if (isAdjacentStep)
                {
                    p->BeginMoveTo(target);
                }
                else
                {
                    p->SetCell(target);
                }
            }
        }
        if (payload.contains("buttons"))
        {
            for (const auto &j : payload["buttons"])
            {
                int id = j.value("id", -1);
                auto *b = findById(const_cast<std::vector<ButtonObject *> &>(runtime->GetButtons()), id);
                if (!b) continue;
                b->SetActive(j.value("active", false));
            }
        }
        if (payload.contains("doors"))
        {
            for (const auto &j : payload["doors"])
            {
                int id = j.value("id", -1);
                auto *d = findById(const_cast<std::vector<DoorObject *> &>(runtime->GetDoors()), id);
                if (!d) continue;
                d->SetOpen(j.value("open", false));
            }
        }
        if (payload.contains("plates"))
        {
            for (const auto &j : payload["plates"])
            {
                int id = j.value("id", -1);
                auto *pl = findById(const_cast<std::vector<PressurePlate *> &>(runtime->GetPlates()), id);
                if (!pl) continue;
                pl->SetActive(j.value("active", false));
            }
        }
        if (payload.contains("players"))
        {
            for (const auto &j : payload["players"])
            {
                int idx = j.value("idx", -1);
                PuzzlePlayer *pp = nullptr;
                if (localPlayer && localPlayer->GetPlayerIndex() == idx) pp = localPlayer;
                else if (remotePlayer && remotePlayer->GetPlayerIndex() == idx) pp = remotePlayer;
                if (!pp) continue;
                if (j.contains("inv") && j["inv"].is_array() && j["inv"].size() == PuzzlePlayer::kInventorySize)
                {
                    std::array<int, PuzzlePlayer::kInventorySize> inv{};
                    std::array<int, PuzzlePlayer::kInventorySize> invIds{};
                    for (int k = 0; k < PuzzlePlayer::kInventorySize; ++k) inv[k] = j["inv"][k].get<int>();
                    if (j.contains("invIds") && j["invIds"].is_array() && j["invIds"].size() == PuzzlePlayer::kInventorySize)
                    {
                        for (int k = 0; k < PuzzlePlayer::kInventorySize; ++k) invIds[k] = j["invIds"][k].get<int>();
                    }
                    pp->SetInventoryState(inv, invIds);
                }
            }
        }
    }

    void GameplayScene::ApplyReplicatedPlayerState(int playerIndex, int facing, Cell targetCell)
    {
        PuzzlePlayer *player = nullptr;
        if (localPlayer && localPlayer->GetPlayerIndex() == playerIndex)
        {
            player = localPlayer;
        }
        else if (remotePlayer && remotePlayer->GetPlayerIndex() == playerIndex)
        {
            player = remotePlayer;
        }

        if (!player)
        {
            return;
        }

        player->SetFacing(facing);
        Cell currentCell = player->GetCell();
        int dx = targetCell.x - currentCell.x;
        int dy = targetCell.y - currentCell.y;
        bool isAdjacentStep = (std::abs(dx) + std::abs(dy) == 1);

        if (targetCell == currentCell)
        {
            return;
        }

        if (isAdjacentStep && !player->IsMoving())
        {
            player->BeginMoveTo(targetCell);
            if (player == localPlayer) pendingNetMove = false;
            runtime->HandlePlayerArrived(*player);
            runtime->RebuildPlateStates();
            return;
        }

        player->TeleportToCell(targetCell);
        if (player == localPlayer) pendingNetMove = false;
        runtime->HandlePlayerArrived(*player);
        runtime->RebuildPlateStates();
    }

    void GameplayScene::UpdateHud()
    {
        if (!hudLabel) return;
        // Recompute UI positions based on current window size so inventory
        // remains anchored to the top-right and dialog stays centered bottom.
        glm::vec2 windowSize = CAM_MN.GetBaseWindowSize();
        const float margin = 20.0f;
        const float extraLeft = 40.0f; // shift further left per user request
        // Inventory anchored top-right, shifted left
        inventoryBasePos = glm::vec2(windowSize.x - margin - inventoryBaseSize.x - extraLeft, margin);
        if (inventoryBaseImage) inventoryBaseImage->SetPosition(inventoryBasePos);
        // Label below inventory cell
        if (inventorySlotLabel)
        {
            glm::vec2 labelSize = glm::vec2(200.0f, 24.0f);
            glm::vec2 labelPos = glm::vec2(inventoryBasePos.x + inventoryBaseSize.x * 0.5f - labelSize.x * 0.5f,
                                           inventoryBasePos.y + inventoryBaseSize.y + 6.0f);
            inventorySlotLabel->SetPosition(labelPos);
        }
        // Dialog message bottom-centered
        if (dialogMessageBg && dialogMessageText)
        {
            glm::vec2 msgSize = dialogMessageBg->GetSize();
            // scale down if too wide for window
            float availableW = std::max(100.0f, windowSize.x - 40.0f);
            if (msgSize.x > availableW)
            {
                float scale = availableW / msgSize.x;
                msgSize.x *= scale;
                msgSize.y *= scale;
            }
            glm::vec2 msgPos = glm::vec2(windowSize.x * 0.5f - msgSize.x * 0.5f,
                                         windowSize.y - msgSize.y - 30.0f);
            dialogMessageBg->SetPosition(msgPos);
            dialogMessageBg->SetSize(msgSize);
            dialogMessageText->SetPosition(msgPos);
            dialogMessageText->SetSize(msgSize);
            dialogMessageText->SetTextColor(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        auto &session = GameSession::Get();
        std::string mode = session.mode == GameSession::NetworkMode::SinglePlayer ? "1P" :
                           session.mode == GameSession::NetworkMode::Host         ? "Host" : "Cliente";
        hudLabel->SetText("Nivel: " + (session.selectedLevel.empty() ? std::string("-") : session.selectedLevel) +
                  "   Modo: " + mode);

        if (inventoryBaseImage && localPlayer)
        {
            // Draw overlay sprite for the single-slot inventory if occupied.
            int tid = localPlayer->PeekInventoryAt(0);
            if (tid != 0)
            {
                // Compute inner draw rect used by UIImage to align overlay exactly.
                const unsigned int texWidth = 1024;
                const unsigned int texHeight = 1024;
                glm::vec2 drawPos = inventoryBasePos;
                glm::vec2 drawSize = inventoryBaseSize;
                if (texWidth > 0 && texHeight > 0)
                {
                    const float texAspect = static_cast<float>(texWidth) / static_cast<float>(texHeight);
                    const float boxAspect = inventoryBaseSize.x / inventoryBaseSize.y;
                    if (texAspect > boxAspect)
                    {
                        drawSize.x = inventoryBaseSize.x;
                        drawSize.y = inventoryBaseSize.x / texAspect;
                    }
                    else
                    {
                        drawSize.y = inventoryBaseSize.y;
                        drawSize.x = inventoryBaseSize.y * texAspect;
                    }
                    drawPos.x = inventoryBasePos.x + (inventoryBaseSize.x - drawSize.x) * 0.5f;
                    drawPos.y = inventoryBasePos.y + (inventoryBaseSize.y - drawSize.y) * 0.5f;
                }
                if (inventoryOverlay)
                {
                    inventoryOverlay->SetPosition(drawPos);
                    inventoryOverlay->SetSize(drawSize);
                }

                int objId = localPlayer->PeekInventoryObjectIdAt(0);
                // Prefer original pickup's sheet variant if available.
                bool overlaySet = false;
                if (objId > 0)
                {
                    PickupObject *pickup = nullptr;
                    for (auto *candidate : runtime->GetPickups())
                    {
                        if (candidate && candidate->GetObjectId() == objId)
                        {
                            pickup = candidate;
                            break;
                        }
                    }
                    if (pickup && pickup->GetSpriteSheet() > 0)
                    {
                        InventoryOverlayElement *ov = dynamic_cast<InventoryOverlayElement *>(inventoryOverlay);
                        if (ov) ov->SetSheetVariant(pickup->GetSpriteSheet(), pickup->GetSpriteRow(), pickup->GetSpriteCol());
                        overlaySet = true;
                    }
                }

                if (!overlaySet)
                {
                    const char *clip = ClipForTextureId(tid);
                    if (clip && clip[0])
                    {
                        if (inventoryOverlay)
                        {
                            InventoryOverlayElement *ov = dynamic_cast<InventoryOverlayElement *>(inventoryOverlay);
                            if (ov) ov->SetClipName(std::string(clip));
                        }
                    }
                }
            }
            else if (inventoryOverlay)
            {
                inventoryOverlay->SetVisible(false);
            }

            // Debug: draw small text showing inventory tid/object id.
            RS_MN.RenderText(std::string("DBG: tid=" + std::to_string(tid) + " obj=" + std::to_string(localPlayer->PeekInventoryObjectIdAt(0))),
                             "puzzle_font",
                             glm::vec3(inventoryBasePos.x, inventoryBasePos.y - 16.0f, 0.0f),
                             glm::vec3(0.5f),
                             glm::vec3(0.0f),
                             glm::vec3(0.0f),
                             glm::vec3(1.0f),
                             1.0f);
        }
    }

    void GameplayScene::HandleLevelCompleted()
    {
        if (state == PlayState::Completed) return;
        if (networkSetup && NET_MN.IsServer())
        {
            NET_MN.Broadcast(CosmicEngine::NetworkMessage(Net::Msg_GameEnd, {{"reason", "win"}}));
        }
        AUD_MN.Stop(PUZZLERPG_MUSIC_GAMEPLAY_KEY);
        SetState(PlayState::Completed);
    }

    void GameplayScene::OnTeleporterStep(PuzzlePlayer &player, TeleporterObject &teleporter)
    {
        // Solo el host (o singleplayer) programa el teletransporte. El cliente
        // recibira el resultado via Msg_PlayerState.
        if (networkSetup && NET_MN.IsClient()) return;
        TeleporterObject *partner = runtime->GetTeleporterPartner(teleporter);
        if (!partner) return;

        // Marca el origen para no re-disparar al volver a aparecer encima.
        player.SetLastTeleporterId(teleporter.GetObjectId());
        player.Freeze(0.5f);
        pendingTeleport            = true;
        pendingTeleportTimer       = 0.5f;
        pendingTeleportPlayerIndex = player.GetPlayerIndex();
        pendingTeleportTarget      = partner->GetCell();
        pendingTeleportTeleId      = partner->GetObjectId();
    }
}
