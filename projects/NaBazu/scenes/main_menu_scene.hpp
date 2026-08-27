#ifndef NABAZU_MAIN_MENU_SCENE_HPP
#define NABAZU_MAIN_MENU_SCENE_HPP

#include "../ui/ui_link_button.hpp"

#include <CosmicEngine/models/scene/scene.hpp>

#include <glm/glm.hpp>

#include <string>

namespace CosmicEngine
{
    class UIText;
}

namespace NaBazu
{
    // Arcade title screen: title, blinking "press start", session record, and buttons
    // into the reference (GameInfoScene) and attribution (CreditsScene) screens.
    class MainMenuScene : public CosmicEngine::Scene
    {
    public:
        MainMenuScene();

        void init() override;
        void update(double deltaTime) override;
        void loadResources() override;

    private:
        UILinkButton *AddMenuButton(const std::string &label, float x);

        CosmicEngine::UIText *promptText_;
        UILinkButton *infoButton_;
        UILinkButton *creditsButton_;
        CosmicEngine::UIText *displayModeText_;
        double blinkElapsed_;
    };
}

#endif
