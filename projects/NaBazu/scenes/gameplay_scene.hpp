#ifndef NABAZU_GAMEPLAY_SCENE_HPP
#define NABAZU_GAMEPLAY_SCENE_HPP

#include "../systems/chase_camera.hpp"
#include "../systems/level_director.hpp"
#include "../systems/rail_path.hpp"
#include "../systems/skybox.hpp"
#include "../systems/test_mode.hpp"
#include "../systems/wave_spawner.hpp"
#include "../ui/ui_gauge_bar.hpp"

#include <CosmicEngine/models/scene/scene.hpp>

#include <memory>
#include <string>

namespace CosmicEngine
{
    class UIText;
}

namespace NaBazu
{
    class Ship;

    class GameplayScene : public CosmicEngine::Scene
    {
    public:
        GameplayScene();
        ~GameplayScene() override;

        void init() override;
        void draw() override;
        void update(double deltaTime) override;
        void loadResources() override;

    private:
        void SetupCollisionArea();
        void RecenterCollisionArea();
        void SetupHud();
        void UpdateHud();
        void UpdateAudioListener();
        void ApplyTestMode(bool enabled);
        void DespawnStaleEnemies();

        std::unique_ptr<RailPath> rail_;
        std::unique_ptr<WaveSpawner> waveSpawner_;
        std::unique_ptr<LevelDirector> levelDirector_;
        std::unique_ptr<Skybox> skybox_;
        ChaseCamera chaseCamera_;
        OrbitCamera orbitCamera_; // TEMPORARY: used instead of chaseCamera_ in test mode
        Ship *ship_;
        int score_;
        int kills_;
        float elapsedTime_;

        CosmicEngine::UIText *hudScoreText_;
        CosmicEngine::UIText *hudDistanceText_;
        CosmicEngine::UIText *hudKillsText_;
        CosmicEngine::UIText *hudSpeedText_;
        CosmicEngine::UIText *hudTargetText_;
        CosmicEngine::UIText *hudDisplayModeText_;
        // Purpose-built read-only gauges (see ui/ui_gauge_bar.hpp) rather than
        // UISlider: a slider is an interactive control, so using one as a readout
        // meant fighting its drag/focus behaviour and showing a meaningless handle.
        UIGaugeBar *healthGauge_;
        UIGaugeBar *brakeGauge_;
        UIGaugeBar *ammoGauge_;
        UIGaugeBar *missileGauge_;
    };
}

#endif
