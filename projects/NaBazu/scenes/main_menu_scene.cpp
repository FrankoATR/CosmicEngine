#include "main_menu_scene.hpp"

#include "credits_scene.hpp"
#include "game_info_scene.hpp"
#include "gameplay_scene.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_display.hpp"
#include "../utilities/nabazu_input_actions.hpp"
#include "../utilities/session_stats.hpp"
#include "../ui/ui_link_button.hpp"

#include <CosmicEngine/models/ui/derived/ui_text.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include <cmath>
#include <string>

namespace
{
    constexpr glm::vec2 kButtonSize(420.0f, 62.0f);
    constexpr float kButtonY = 700.0f;
}

namespace NaBazu
{
    MainMenuScene::MainMenuScene()
        : Scene("MainMenuScene"),
          promptText_(nullptr),
          infoButton_(nullptr),
          creditsButton_(nullptr),
          displayModeText_(nullptr),
          blinkElapsed_(0.0)
    {
    }

    void MainMenuScene::loadResources()
    {
        RS_MN.LoadFont(NABAZU_FONT_MAIN_KEY, NABAZU_FONT_MAIN_PATH, 32);
    }

    UILinkButton *MainMenuScene::AddMenuButton(const std::string &label, float x)
    {
        auto *button = new UILinkButton(label, NABAZU_FONT_MAIN_KEY, glm::vec2(x, kButtonY), kButtonSize);
        button->SetTextColor(glm::vec3(0.7f, 0.78f, 0.86f));
        button->SetSelectedTextColor(glm::vec3(1.0f, 0.95f, 0.35f));
        UI_MN.AddElement(button);
        return button;
    }

    void MainMenuScene::init()
    {
        RegisterNaBazuInputActions();

        SCN_MN.SetBackgroundColor(glm::vec3(0.015f, 0.02f, 0.04f));

        // The reference text that used to crowd this screen now lives in
        // GameInfoScene, which frees the title screen to breathe and use real scale.
        auto *title = new CosmicEngine::UIText("NA BAZU", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 150.0f), glm::vec2(1920.0f, 160.0f), true);
        title->SetTextScale(3.2f);
        title->SetTextColor(glm::vec3(0.25f, 0.85f, 1.0f));
        UI_MN.AddElement(title);

        auto *subtitle = new CosmicEngine::UIText("Rail shooter infinito | sobrevive y suma puntos",
            NABAZU_FONT_MAIN_KEY, glm::vec2(0.0f, 330.0f), glm::vec2(1920.0f, 50.0f), true);
        subtitle->SetTextScale(1.15f);
        subtitle->SetTextColor(glm::vec3(0.6f, 0.7f, 0.75f));
        UI_MN.AddElement(subtitle);

        promptText_ = new CosmicEngine::UIText("PULSA ENTER PARA JUGAR", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 460.0f), glm::vec2(1920.0f, 70.0f), true);
        promptText_->SetTextScale(1.7f);
        promptText_->SetTextColor(glm::vec3(1.0f, 0.95f, 0.3f));
        UI_MN.AddElement(promptText_);

        auto *record = new CosmicEngine::UIText("Mejor puntaje de la sesion: " + std::to_string(GetSessionHighScore()),
            NABAZU_FONT_MAIN_KEY, glm::vec2(0.0f, 570.0f), glm::vec2(1920.0f, 50.0f), true);
        record->SetTextScale(1.15f);
        record->SetTextColor(glm::vec3(0.85f, 0.8f, 0.45f));
        UI_MN.AddElement(record);

        // UILinkButton (not UIButton) so these never take keyboard focus -- a focusable
        // button here permanently swallowed ENTER and blocked starting the game.
        infoButton_ = AddMenuButton("INFORMACION DE JUEGO  [I]", 490.0f);
        infoButton_->SetOnClick([]() { SCN_MN.ReplaceScene(new GameInfoScene()); });

        creditsButton_ = AddMenuButton("CREDITOS  [C]", 1010.0f);
        creditsButton_->SetOnClick([]() { SCN_MN.ReplaceScene(new CreditsScene()); });

        // Live display state, refreshed in update() as F11 is pressed.
        displayModeText_ = new CosmicEngine::UIText("", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 1010.0f), glm::vec2(1920.0f, 34.0f), true);
        displayModeText_->SetTextColor(glm::vec3(0.45f, 0.5f, 0.58f));
        UI_MN.AddElement(displayModeText_);
    }

    void MainMenuScene::update(double deltaTime)
    {
        blinkElapsed_ += deltaTime;

        if (promptText_)
        {
            promptText_->SetVisible(std::fmod(blinkElapsed_, 1.0) < 0.6);
        }

        if (displayModeText_)
        {
            displayModeText_->SetText("F11: " + DescribeDisplayMode());
        }

        HandleDisplayToggles();

        if (INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
        {
            GM_MN.endprogram();
        }

        // Keyboard shortcuts alongside the buttons: the rest of this menu is
        // keyboard-driven, so requiring the mouse for these would be odd.
        if (INP_MN.IsKeyPressed(GLFW_KEY_I, CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new GameInfoScene());
            return;
        }

        if (INP_MN.IsKeyPressed(GLFW_KEY_C, CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new CreditsScene());
            return;
        }

        if (INP_MN.IsActionPressed(kActionConfirm, CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new GameplayScene());
        }
    }
}
