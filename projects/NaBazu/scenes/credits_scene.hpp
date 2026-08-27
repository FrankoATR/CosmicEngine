#ifndef NABAZU_CREDITS_SCENE_HPP
#define NABAZU_CREDITS_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>

#include <glm/glm.hpp>

#include <string>

namespace CosmicEngine
{
    class UIText;
}

namespace NaBazu
{
    // Attribution screen reached from the main menu: each section heading with the
    // credited name underneath. Returns to the menu on confirm or cancel, so either
    // key gets the player out.
    class CreditsScene : public CosmicEngine::Scene
    {
    public:
        CreditsScene();

        void init() override;
        void update(double deltaTime) override;
        void loadResources() override;

    private:
        void AddCreditRow(const std::string &role, const std::string &author, float y);

        CosmicEngine::UIText *displayModeText_;
    };
}

#endif
