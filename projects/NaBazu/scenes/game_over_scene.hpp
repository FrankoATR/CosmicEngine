#ifndef NABAZU_GAME_OVER_SCENE_HPP
#define NABAZU_GAME_OVER_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>

#include <glm/glm.hpp>

#include <string>

namespace CosmicEngine
{
    class UIText;
}

namespace NaBazu
{
    // Shown when the ship's health reaches 0 during an infinite/score-attack run.
    // Breaks the run down (score, kills, distance, accuracy-ish weapon usage) and
    // compares it against the session records, then offers retry or return to menu.
    class GameOverScene : public CosmicEngine::Scene
    {
    public:
        GameOverScene(int finalScore, int kills, int distance);

        void init() override;
        void update(double deltaTime) override;
        void loadResources() override;

    private:
        void AddStatRow(const std::string &label, const std::string &value, float y, glm::vec3 valueColor);

        int finalScore_;
        int kills_;
        int distance_;
        bool isNewRecord_;

        CosmicEngine::UIText *promptText_;
        CosmicEngine::UIText *displayModeText_;
        double blinkElapsed_;
    };
}

#endif
