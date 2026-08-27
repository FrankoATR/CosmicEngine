#include "gameplay_scene.hpp"

#include "../entities/enemy_grunt.hpp"
#include "../entities/ship.hpp"
#include "../systems/glow_mesh_renderer.hpp"
#include "../systems/nabazu_level_data.hpp"
#include "../systems/test_mode.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_input_actions.hpp"
#include "../utilities/nabazu_models.hpp"
#include "game_over_scene.hpp"

#include "../ui/ui_gauge_bar.hpp"
#include "../utilities/nabazu_display.hpp"

#include <CosmicEngine/collisions/collision_area.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include CAMERAMANAGER_HEADER
#include EVENTMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include <string>

namespace
{
    // Fase 2: level is infinite, so collision can no longer use one static box sized
    // to the whole corridor -- instead a fixed-size window recentered on the ship
    // every tick. Confirmed safe: BodyManager::update() clears and fully re-inserts
    // every Body into the grid every tick regardless, so there is no stale cell
    // membership that repositioning could corrupt (see plan, Fase 2 "Verificacion de
    // riesgo clave"). Sized comfortably around the new lateralOffsetMax_(42)/
    // verticalOffsetMax_(30) plus splash radius and the wave/rail lookahead margins.
    // Sized from what actually has to be collidable at once, NOT from the visible
    // play tube: GameGridCollisions silently drops any body whose box falls outside
    // the grid (GetCellByPositionAndSize returns nullptr -> no collisions at all for
    // it). The old 440x100x440 window was far too small -- measured in-game, 40-65%
    // of live bullets and occasionally whole enemy formations sat outside it and were
    // simply non-collidable, which is why shots visibly passed through enemies.
    //
    // Half-extents now: +/-600 X/Z (bullets travel ~234 u/s relative to the ship, so
    // they expire well before reaching the edge) and +/-300 Y (covers the steepest
    // ClimbDive rail segment plus the ship's own +/-30 vertical range).
    const glm::vec3 kCollisionWindowSize(1200.0f, 600.0f, 1200.0f);
    // Cell size scales with the window so the cell count stays modest
    // (12 x 6 x 12 = 864 cells); bodies here are small and few, so coarse cells cost
    // nothing while keeping the per-frame 27-neighbour sweep cheap.
    constexpr int kCollisionCellSize = 100;
    constexpr float kEnemyDespawnBehindMargin = 150.0f;
    constexpr float kProceduralWaveStartDistance = 1550.0f; // just past GetWaveTable()'s last entry (1350)
}

namespace NaBazu
{
    GameplayScene::GameplayScene()
        : Scene("GameplayScene"),
          ship_(nullptr),
          score_(0),
          kills_(0),
          elapsedTime_(0.0f),
          hudScoreText_(nullptr),
          hudDistanceText_(nullptr),
          hudKillsText_(nullptr),
          hudSpeedText_(nullptr),
          hudTargetText_(nullptr),
          hudDisplayModeText_(nullptr),
          healthGauge_(nullptr),
          brakeGauge_(nullptr),
          ammoGauge_(nullptr),
          missileGauge_(nullptr)
    {
    }

    GameplayScene::~GameplayScene()
    {
        // Release the mouse callbacks that capture orbitCamera_ (see OrbitCamera::Detach).
        orbitCamera_.Detach();
    }

    void GameplayScene::ApplyTestMode(bool enabled)
    {
        SetTestModeEnabled(enabled);

        if (enabled)
        {
            orbitCamera_.Attach();
            if (ship_)
            {
                ship_->SetAutoAdvanceEnabled(false); // J/K drive the rail position instead
            }
            return;
        }

        orbitCamera_.Detach();

        // Leaving inspection must undo everything it changed, or normal play resumes
        // with a stalled ship and a world full of frozen entities.
        if (IsWorldFrozen())
        {
            ToggleWorldFrozen();
        }
        if (ship_)
        {
            ship_->SetAutoAdvanceEnabled(true);
        }
        chaseCamera_.Reset();
    }

    void GameplayScene::loadResources()
    {
        RS_MN.LoadTexture(NABAZU_TEXTURE_BULLET_KEY, NABAZU_TEXTURE_BULLET_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_ENEMY_BULLET_KEY, NABAZU_TEXTURE_ENEMY_BULLET_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_MISSILE_KEY, NABAZU_TEXTURE_MISSILE_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_PICKUP_AMMO_KEY, NABAZU_TEXTURE_PICKUP_AMMO_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_PICKUP_MISSILE_KEY, NABAZU_TEXTURE_PICKUP_MISSILE_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_EXPLOSION_KEY, NABAZU_TEXTURE_EXPLOSION_PATH);
        RS_MN.LoadTexture(NABAZU_TEXTURE_RETICLE_KEY, NABAZU_TEXTURE_RETICLE_PATH);
        LoadAllShipModels();
        RS_MN.LoadFont(NABAZU_FONT_MAIN_KEY, NABAZU_FONT_MAIN_PATH, 32);
        AUD_MN.Load(NABAZU_MUSIC_GAMEPLAY_KEY, NABAZU_MUSIC_GAMEPLAY_PATH, CosmicEngine::SoundType::Music);

        AUD_MN.Load(NABAZU_SFX_SHOOT_KEY, NABAZU_SFX_SHOOT_PATH, CosmicEngine::SoundType::SFX);
        AUD_MN.Load(NABAZU_SFX_BOMB_KEY, NABAZU_SFX_BOMB_PATH, CosmicEngine::SoundType::SFX);
        AUD_MN.Load(NABAZU_SFX_BOMB_DESTROY_KEY, NABAZU_SFX_BOMB_DESTROY_PATH, CosmicEngine::SoundType::SFX);
        AUD_MN.Load(NABAZU_SFX_ENEMY_DESTROY_KEY, NABAZU_SFX_ENEMY_DESTROY_PATH, CosmicEngine::SoundType::SFX);
        AUD_MN.Load(NABAZU_SFX_PLAYER_DESTROY_KEY, NABAZU_SFX_PLAYER_DESTROY_PATH, CosmicEngine::SoundType::SFX);
        AUD_MN.Load(NABAZU_SFX_PICKUP_KEY, NABAZU_SFX_PICKUP_PATH, CosmicEngine::SoundType::SFX);
    }

    void GameplayScene::init()
    {
        // loadResources() is already called once by SceneManager before init() --
        // calling it again here would double-load every texture/font/sound.
        RegisterNaBazuInputActions();

        SCN_MN.SetBackgroundColor(glm::vec3(0.02f, 0.02f, 0.05f));

        rail_ = std::make_unique<RailPath>(GetCorridorWaypoints());

        ship_ = new Ship(rail_.get());
        OBJ_MN.Add(ship_);

        // Must run after ship_ exists so the initial collision window is centered on
        // its real spawn position (Fase 1 created the area before the ship; the
        // infinite level's sliding window needs the opposite order).
        SetupCollisionArea();

        score_ = 0;
        kills_ = 0;
        EVT_MN.RegisterEventListener<int>("nabazu.enemy_killed",
            std::function<void(int)>([this](int points) { score_ += points; ++kills_; }));

        waveSpawner_ = std::make_unique<WaveSpawner>(rail_.get(), GetWaveTable());
        levelDirector_ = std::make_unique<LevelDirector>(rail_.get(), waveSpawner_.get(), kProceduralWaveStartDistance);
        skybox_ = std::make_unique<Skybox>();

        // Force-compile the glow shaders now (they are otherwise lazily built on
        // first draw of a Bullet/Missile/ExplosionEffect) so a bad shader fails fast
        // here instead of causing a first-shot/first-kill hitch mid-gameplay.
        BoltGlowRenderer::GetInstance();
        OrbGlowRenderer::GetInstance();
        ExplosionRenderer::GetInstance();
        CrosshairRenderer::GetInstance();
        StarPickupRenderer::GetInstance();
        RingPickupRenderer::GetInstance();

        CAM_MN.SetZoom(55.0f);

        // TEMPORARY collision-inspection mode (systems/test_mode.hpp), toggled with F3.
        // Re-applied here so the mode survives a scene restart (retry / new run).
        ApplyTestMode(IsTestModeEnabled());

        SetupHud();

        // Plenty of overlapping voices: rapid fire alone can stack several shots.
        AUD_MN.SetSfxVoicesPerSound(24);

        // World scale here is hundreds of units, so miniaudio's default 1-unit
        // rolloff would make everything but point-blank sounds inaudible. Full volume
        // out to 60 units, fading to the 500-unit cap (past the ~260u aim range and
        // the wave lookahead, so nothing pops in silently).
        AUD_MN.SetSpatialAttenuation(60.0f, 500.0f, 1.0f);

        AUD_MN.Play(NABAZU_MUSIC_GAMEPLAY_KEY, 0.75f, true);
    }

    void GameplayScene::SetupCollisionArea()
    {
        const glm::vec3 initialCenter = ship_ ? ship_->GetPosition() : glm::vec3(0.0f);
        BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid,
            initialCenter - kCollisionWindowSize * 0.5f, kCollisionWindowSize, kCollisionCellSize, 5, 6);
    }

    void GameplayScene::RecenterCollisionArea()
    {
        if (ship_)
        {
            BOD_MN.SetGridPosition(ship_->GetPosition() - kCollisionWindowSize * 0.5f);
        }
    }

    void GameplayScene::UpdateAudioListener()
    {
        // The listener rides the CAMERA, not the ship: panning has to match what is
        // on screen, and screen-left/right is defined by where the camera looks. The
        // engine only tracked listener position before (see AudioManager), which left
        // panning wrong whenever the rail curved the view away from -Z.
        const glm::vec3 front = CAM_MN.GetViewDirection();
        const glm::vec3 right = CAM_MN.GetRightDirection();
        const glm::vec3 up = glm::cross(right, front);

        AUD_MN.SetListenerPosition(CAM_MN.GetPosition());
        AUD_MN.SetListenerDirection(front, up);
    }

    void GameplayScene::DespawnStaleEnemies()
    {
        if (!ship_)
        {
            return;
        }

        const float shipDistance = ship_->GetDistanceTraveled();
        for (auto *object : OBJ_MN.FindByClassName(EnemyGrunt::StaticClassName()))
        {
            if (auto *enemy = static_cast<EnemyGrunt *>(object))
            {
                enemy->MaybeDespawnBehind(shipDistance, kEnemyDespawnBehindMargin);
            }
        }
    }

    void GameplayScene::SetupHud()
    {
        // --- Stat readouts (top-left, left-aligned) -------------------------------
        auto makeStat = [](const char *initial, float y, glm::vec3 color, float height)
        {
            // leftAlign=true: UIText centers within its box by default, which would
            // drift each stat horizontally as its text length changed.
            auto *text = new CosmicEngine::UIText(initial, NABAZU_FONT_MAIN_KEY,
                glm::vec2(36.0f, y), glm::vec2(700.0f, height), true, nullptr, false, 1, true);
            text->SetTextColor(color);
            UI_MN.AddElement(text);
            return text;
        };

        hudScoreText_ = makeStat("", 26.0f, glm::vec3(1.0f, 0.88f, 0.35f), 46.0f);
        hudKillsText_ = makeStat("", 80.0f, glm::vec3(1.0f, 0.55f, 0.5f), 34.0f);
        hudDistanceText_ = makeStat("", 118.0f, glm::vec3(0.7f, 0.9f, 0.75f), 34.0f);
        hudSpeedText_ = makeStat("", 156.0f, glm::vec3(0.65f, 0.85f, 1.0f), 34.0f);

        // Lights up while the aim assist is tracking something, so the help the
        // player is getting is visible rather than invisible magic.
        hudTargetText_ = makeStat("", 194.0f, glm::vec3(1.0f, 0.3f, 0.25f), 34.0f);

        // Current window/vsync state, kept in sync with the F11 toggle.
        hudDisplayModeText_ = new CosmicEngine::UIText("", NABAZU_FONT_MAIN_KEY,
            glm::vec2(1180.0f, 26.0f), glm::vec2(700.0f, 32.0f), true, nullptr, false, 1, false);
        hudDisplayModeText_->SetTextColor(glm::vec3(0.45f, 0.5f, 0.58f));
        UI_MN.AddElement(hudDisplayModeText_);

        // --- Gauges (bottom-left) -------------------------------------------------
        const float gaugeX = 36.0f;
        const glm::vec2 gaugeSize(380.0f, 52.0f);

        healthGauge_ = new UIGaugeBar("INTEGRIDAD", NABAZU_FONT_MAIN_KEY, glm::vec2(gaugeX, 812.0f), gaugeSize, 20);
        healthGauge_->SetBarColor(glm::vec3(0.35f, 0.9f, 0.55f));
        healthGauge_->SetLowColor(glm::vec3(1.0f, 0.25f, 0.2f));
        healthGauge_->SetLowThreshold(0.35f);
        healthGauge_->SetPulseWhenLow(true);
        UI_MN.AddElement(healthGauge_);

        brakeGauge_ = new UIGaugeBar("FRENO", NABAZU_FONT_MAIN_KEY, glm::vec2(gaugeX, 876.0f), gaugeSize, 20);
        brakeGauge_->SetBarColor(glm::vec3(0.3f, 0.75f, 1.0f));
        brakeGauge_->SetLowThreshold(0.0f); // never re-tints: an empty brake bar is normal, not an alert
        UI_MN.AddElement(brakeGauge_);

        ammoGauge_ = new UIGaugeBar("MUNICION", NABAZU_FONT_MAIN_KEY, glm::vec2(gaugeX, 940.0f), gaugeSize, 24);
        ammoGauge_->SetBarColor(glm::vec3(1.0f, 0.85f, 0.25f));
        ammoGauge_->SetLowThreshold(0.2f);
        ammoGauge_->SetPulseWhenLow(true);
        UI_MN.AddElement(ammoGauge_);

        // One segment per missile, so the bar doubles as a countable pip strip.
        missileGauge_ = new UIGaugeBar("MISILES", NABAZU_FONT_MAIN_KEY, glm::vec2(gaugeX, 1004.0f), gaugeSize,
                                        ship_ ? ship_->GetMaxMissiles() : 6);
        missileGauge_->SetBarColor(glm::vec3(0.8f, 0.4f, 1.0f));
        missileGauge_->SetLowThreshold(0.0f);
        UI_MN.AddElement(missileGauge_);
    }

    void GameplayScene::UpdateHud()
    {
        if (!ship_)
        {
            return;
        }

        if (hudScoreText_)
        {
            hudScoreText_->SetText("PUNTAJE  " + std::to_string(score_));
        }

        if (hudKillsText_)
        {
            hudKillsText_->SetText("Derribos: " + std::to_string(kills_));
        }

        if (hudDistanceText_)
        {
            // No "/ total": the level is infinite now, a fixed denominator would mislead.
            hudDistanceText_->SetText("Distancia: " + std::to_string(static_cast<int>(ship_->GetDistanceTraveled())) + " m");
        }

        if (hudSpeedText_)
        {
            const int speed = static_cast<int>(ship_->GetCurrentSpeed());
            const bool braking = ship_->GetCurrentSpeed() < ship_->GetCruiseSpeed() - 1.0f;
            hudSpeedText_->SetText("Velocidad: " + std::to_string(speed) + (braking ? " (frenando)" : ""));
        }

        if (hudTargetText_)
        {
            const bool locked = ship_->GetAimLockStrength() > 0.5f;
            hudTargetText_->SetText(locked ? "> OBJETIVO FIJADO <" : "");
        }

        if (hudDisplayModeText_)
        {
            std::string line = "F11  " + DescribeDisplayMode();
            if (IsTestModeEnabled())
            {
                line += IsWorldFrozen() ? "   |   F3 INSPECCION (CONGELADO)" : "   |   F3 INSPECCION";
            }
            hudDisplayModeText_->SetText(line);
        }

        if (healthGauge_ && ship_->GetMaxHealth() > 0)
        {
            healthGauge_->SetValue(static_cast<float>(ship_->GetHealth()) / static_cast<float>(ship_->GetMaxHealth()));
            healthGauge_->SetReadout(std::to_string(ship_->GetHealth()) + " / " + std::to_string(ship_->GetMaxHealth()));
        }

        if (ammoGauge_ && ship_->GetMaxAmmo() > 0)
        {
            ammoGauge_->SetValue(static_cast<float>(ship_->GetAmmo()) / static_cast<float>(ship_->GetMaxAmmo()));
            ammoGauge_->SetReadout(std::to_string(ship_->GetAmmo()) + " / " + std::to_string(ship_->GetMaxAmmo()));
        }

        if (missileGauge_ && ship_->GetMaxMissiles() > 0)
        {
            missileGauge_->SetValue(static_cast<float>(ship_->GetMissiles()) / static_cast<float>(ship_->GetMaxMissiles()));
            missileGauge_->SetReadout(std::to_string(ship_->GetMissiles()) + " / " + std::to_string(ship_->GetMaxMissiles()));
        }

        if (brakeGauge_)
        {
            const float brakeTarget = ship_->GetBrakeTimerTarget();
            const float brakeElapsed = ship_->GetBrakeTimerElapsed();
            const float brakeProgress = (brakeTarget > 0.0f) ? glm::clamp(brakeElapsed / brakeTarget, 0.0f, 1.0f) : 0.0f;

            float brakeFraction = 1.0f;
            const char *stateLabel = "LISTO";
            switch (ship_->GetBrakeState())
            {
            case BrakeState::Idle:
                brakeFraction = 1.0f;
                stateLabel = "LISTO";
                break;
            case BrakeState::Braking:
                brakeFraction = 1.0f - brakeProgress;
                stateLabel = "FRENANDO";
                break;
            case BrakeState::Recovering:
                brakeFraction = brakeProgress;
                stateLabel = "RECUPERANDO";
                break;
            case BrakeState::Cooldown:
                brakeFraction = brakeProgress;
                stateLabel = "RECARGANDO";
                break;
            }

            brakeGauge_->SetValue(brakeFraction);
            brakeGauge_->SetReadout(stateLabel);
        }
    }

    void GameplayScene::draw()
    {
        if (skybox_)
        {
            skybox_->Draw(elapsedTime_);
        }
    }

    void GameplayScene::update(double deltaTime)
    {
        const float dt = static_cast<float>(deltaTime);
        elapsedTime_ += dt;

        if (INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
        }

        if (INP_MN.IsActionPressed(kActionToggleDebugBodies, CosmicEngine::KeyDown))
        {
            ToogleShowBodys();
        }

        HandleDisplayToggles();

        // Ship death ends the run (no more "reach the destination" -- infinite,
        // score-attack). ReplaceScene() is deferred (see scene_manager.cpp), so it is
        // safe that this scene keeps running for the remainder of this tick, but we
        // bail out here anyway to avoid doing further pointless work on a dead run.
        if (ship_ && ship_->GetHealth() <= 0)
        {
            SCN_MN.ReplaceScene(new GameOverScene(score_, kills_, static_cast<int>(ship_->GetDistanceTraveled())));
            return;
        }

        if (INP_MN.IsKeyPressed(GLFW_KEY_F3, CosmicEngine::KeyDown))
        {
            ApplyTestMode(!IsTestModeEnabled());
        }

        if (ship_ && rail_)
        {
            if (IsTestModeEnabled())
            {
                // Manual rail stepping so a spot can be parked on and studied.
                constexpr float kManualStepSpeed = 60.0f;
                if (INP_MN.IsKeyPressed(GLFW_KEY_J, CosmicEngine::KeyRelease))
                {
                    ship_->NudgeRailDistance(kManualStepSpeed * dt);
                }
                if (INP_MN.IsKeyPressed(GLFW_KEY_K, CosmicEngine::KeyRelease))
                {
                    ship_->NudgeRailDistance(-kManualStepSpeed * dt);
                }
                if (INP_MN.IsKeyPressed(GLFW_KEY_O, CosmicEngine::KeyDown))
                {
                    ToggleWorldFrozen();
                }

                orbitCamera_.Update(*ship_);
            }
            else
            {
                chaseCamera_.Update(*ship_, *rail_, dt);
            }
        }

        if (waveSpawner_ && ship_)
        {
            waveSpawner_->Update(ship_->GetDistanceTraveled());
        }

        if (levelDirector_ && ship_)
        {
            levelDirector_->Update(ship_->GetDistanceTraveled());
        }

        RecenterCollisionArea();
        DespawnStaleEnemies();

        UpdateAudioListener();

        UpdateHud();
    }
}
