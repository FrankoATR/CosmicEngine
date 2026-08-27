#ifndef NABAZU_GAME_INFO_SCENE_HPP
#define NABAZU_GAME_INFO_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>

#include <glm/glm.hpp>

#include <string>

namespace CosmicEngine
{
    class UIText;
}

namespace NaBazu
{
    // Reference screen reached from the main menu: controls in one column, pickups and
    // tips in the other. Split out of the title screen so the menu itself stays a
    // clean arcade attract screen instead of a wall of text.
    class GameInfoScene : public CosmicEngine::Scene
    {
    public:
        GameInfoScene();

        void init() override;
        void update(double deltaTime) override;
        void loadResources() override;

    private:
        void AddLine(const std::string &text, float x, float y, glm::vec3 color);
        void AddHeading(const std::string &text, float x, float y);

        CosmicEngine::UIText *displayModeText_;
    };
}

#endif
