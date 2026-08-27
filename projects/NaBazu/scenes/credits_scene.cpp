#include "credits_scene.hpp"

#include "main_menu_scene.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_display.hpp"
#include "../utilities/nabazu_input_actions.hpp"

#include <CosmicEngine/models/ui/derived/ui_text.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include INPUTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include <string>

namespace
{
    constexpr float kFirstEntryY = 250.0f;
    constexpr float kEntryHeight = 130.0f;
    constexpr float kNameOffsetY = 54.0f;
}

namespace NaBazu
{
    CreditsScene::CreditsScene()
        : Scene("CreditsScene"),
          displayModeText_(nullptr)
    {
    }

    void CreditsScene::loadResources()
    {
        RS_MN.LoadFont(NABAZU_FONT_MAIN_KEY, NABAZU_FONT_MAIN_PATH, 32);
    }

    void CreditsScene::AddCreditRow(const std::string &role, const std::string &author, float y)
    {
        // Section heading, with the credited name centred underneath it.
        auto *roleText = new CosmicEngine::UIText(role, NABAZU_FONT_MAIN_KEY, glm::vec2(0.0f, y),
                                                   glm::vec2(1920.0f, 44.0f), true);
        roleText->SetTextScale(1.05f);
        roleText->SetTextColor(glm::vec3(0.4f, 0.55f, 0.68f));
        UI_MN.AddElement(roleText);

        auto *authorText = new CosmicEngine::UIText(author, NABAZU_FONT_MAIN_KEY,
                                                     glm::vec2(0.0f, y + kNameOffsetY),
                                                     glm::vec2(1920.0f, 52.0f), true);
        authorText->SetTextScale(1.45f);
        authorText->SetTextColor(glm::vec3(0.95f, 0.96f, 1.0f));
        UI_MN.AddElement(authorText);
    }

    void CreditsScene::init()
    {
        RegisterNaBazuInputActions();

        SCN_MN.SetBackgroundColor(glm::vec3(0.015f, 0.02f, 0.04f));

        auto *title = new CosmicEngine::UIText("CREDITOS", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 110.0f), glm::vec2(1920.0f, 90.0f), true);
        title->SetTextScale(2.2f);
        title->SetTextColor(glm::vec3(0.25f, 0.85f, 1.0f));
        UI_MN.AddElement(title);

        float y = kFirstEntryY;
        AddCreditRow("SFX", "StupidPlusPlus", y);
        y += kEntryHeight;
        AddCreditRow("MUSICA", "Melody Ayres-Griffiths", y);
        y += kEntryHeight;
        AddCreditRow("MODELOS 3D", "Max Parata", y);
        y += kEntryHeight;
        AddCreditRow("TIPOGRAFIA", "Rick Hoppmann  -  ThaleahFat", y);
        y += kEntryHeight;
        AddCreditRow("PROGRAMACION", "Francisco Rosa", y);

        auto *prompt = new CosmicEngine::UIText("ENTER / ESC: volver al menu", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 930.0f), glm::vec2(1920.0f, 50.0f), true);
        prompt->SetTextScale(1.1f);
        prompt->SetTextColor(glm::vec3(1.0f, 0.95f, 0.3f));
        UI_MN.AddElement(prompt);

        displayModeText_ = new CosmicEngine::UIText("", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 1010.0f), glm::vec2(1920.0f, 34.0f), true);
        displayModeText_->SetTextColor(glm::vec3(0.45f, 0.5f, 0.58f));
        UI_MN.AddElement(displayModeText_);
    }

    void CreditsScene::update(double deltaTime)
    {
        (void)deltaTime;

        if (displayModeText_)
        {
            displayModeText_->SetText("F11: " + DescribeDisplayMode());
        }

        HandleDisplayToggles();

        // Both confirm and cancel go back: this screen has nothing to confirm, so
        // trapping the player behind one specific key would just be friction.
        if (INP_MN.IsActionPressed(kActionConfirm, CosmicEngine::KeyDown) ||
            INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new MainMenuScene());
        }
    }
}
