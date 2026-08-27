#include "game_over_scene.hpp"

#include "gameplay_scene.hpp"
#include "main_menu_scene.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_display.hpp"
#include "../utilities/nabazu_input_actions.hpp"
#include "../utilities/session_stats.hpp"

#include <CosmicEngine/models/ui/derived/ui_text.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include GAMEMANAGE_HEADER
#include AUDIOMANAGER_HEADER
#include INPUTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include <cmath>
#include <string>

namespace
{
    constexpr float kLabelX = 620.0f;
    constexpr float kValueX = 1060.0f;
    constexpr float kColumnWidth = 420.0f;
    constexpr float kRowHeight = 46.0f;
}

namespace NaBazu
{
    GameOverScene::GameOverScene(int finalScore, int kills, int distance)
        : Scene("GameOverScene"),
          finalScore_(finalScore),
          kills_(kills),
          distance_(distance),
          isNewRecord_(false),
          promptText_(nullptr),
          displayModeText_(nullptr),
          blinkElapsed_(0.0)
    {
    }

    void GameOverScene::loadResources()
    {
        RS_MN.LoadFont(NABAZU_FONT_MAIN_KEY, NABAZU_FONT_MAIN_PATH, 32);

        // The death sting is loaded and played HERE rather than in GameplayScene:
        // SceneManager clears every loaded sound when it swaps scenes, so a sound
        // started at the moment of death would be cut off within the same frame.
        AUD_MN.Load(NABAZU_SFX_PLAYER_DESTROY_KEY, NABAZU_SFX_PLAYER_DESTROY_PATH, CosmicEngine::SoundType::SFX);
    }

    void GameOverScene::AddStatRow(const std::string &label, const std::string &value, float y, glm::vec3 valueColor)
    {
        auto *labelText = new CosmicEngine::UIText(label, NABAZU_FONT_MAIN_KEY, glm::vec2(kLabelX, y),
                                                    glm::vec2(kColumnWidth, 36.0f), true, nullptr, false, 1, true);
        labelText->SetTextColor(glm::vec3(0.55f, 0.58f, 0.64f));
        UI_MN.AddElement(labelText);

        auto *valueText = new CosmicEngine::UIText(value, NABAZU_FONT_MAIN_KEY, glm::vec2(kValueX, y),
                                                    glm::vec2(kColumnWidth, 36.0f), true, nullptr, false, 1, true);
        valueText->SetTextColor(valueColor);
        UI_MN.AddElement(valueText);
    }

    void GameOverScene::init()
    {
        RegisterNaBazuInputActions();

        // Record BEFORE reading the stats back, so "record" reflects this run too.
        isNewRecord_ = RecordRun(finalScore_, distance_, kills_);
        const SessionStats &stats = GetSessionStats();

        SCN_MN.SetBackgroundColor(glm::vec3(0.03f, 0.01f, 0.01f));

        // Global (non-spatial): at this point it is a UI sting, not a world event.
        AUD_MN.Play(NABAZU_SFX_PLAYER_DESTROY_KEY, 0.9f, false);

        auto *title = new CosmicEngine::UIText("GAME OVER", NABAZU_FONT_MAIN_KEY, glm::vec2(0.0f, 150.0f), glm::vec2(1920.0f, 90.0f), true);
        title->SetTextColor(glm::vec3(1.0f, 0.25f, 0.2f));
        UI_MN.AddElement(title);

        if (isNewRecord_)
        {
            auto *record = new CosmicEngine::UIText("NUEVO RECORD DE LA SESION", NABAZU_FONT_MAIN_KEY,
                glm::vec2(0.0f, 250.0f), glm::vec2(1920.0f, 48.0f), true);
            record->SetTextColor(glm::vec3(1.0f, 0.9f, 0.35f));
            UI_MN.AddElement(record);
        }

        auto *scoreBig = new CosmicEngine::UIText(std::to_string(finalScore_) + " PTS", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 320.0f), glm::vec2(1920.0f, 72.0f), true);
        scoreBig->SetTextColor(glm::vec3(1.0f, 0.95f, 0.6f));
        UI_MN.AddElement(scoreBig);

        // --- Run breakdown --------------------------------------------------------
        float y = 440.0f;
        AddStatRow("Enemigos derribados", std::to_string(kills_), y, glm::vec3(1.0f, 0.6f, 0.55f));
        y += kRowHeight;
        AddStatRow("Distancia recorrida", std::to_string(distance_) + " m", y, glm::vec3(0.7f, 0.95f, 0.75f));
        y += kRowHeight;

        const int pointsPerKill = (kills_ > 0) ? (finalScore_ / kills_) : 0;
        AddStatRow("Puntos por derribo", std::to_string(pointsPerKill), y, glm::vec3(0.75f, 0.85f, 1.0f));
        y += kRowHeight * 1.6f;

        // --- Session records ------------------------------------------------------
        AddStatRow("Mejor puntaje", std::to_string(stats.highScore), y,
                    isNewRecord_ ? glm::vec3(1.0f, 0.9f, 0.35f) : glm::vec3(0.85f, 0.85f, 0.5f));
        y += kRowHeight;
        AddStatRow("Mejor distancia", std::to_string(stats.bestDistance) + " m", y, glm::vec3(0.7f, 0.8f, 0.7f));
        y += kRowHeight;
        AddStatRow("Mas derribos", std::to_string(stats.bestKills), y, glm::vec3(0.85f, 0.7f, 0.7f));
        y += kRowHeight;
        AddStatRow("Partidas jugadas", std::to_string(stats.runsPlayed), y, glm::vec3(0.65f, 0.65f, 0.7f));

        promptText_ = new CosmicEngine::UIText("ENTER: reintentar     ESC: menu principal", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 880.0f), glm::vec2(1920.0f, 50.0f), true);
        promptText_->SetTextColor(glm::vec3(1.0f, 0.95f, 0.3f));
        UI_MN.AddElement(promptText_);

        displayModeText_ = new CosmicEngine::UIText("", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 1010.0f), glm::vec2(1920.0f, 34.0f), true);
        displayModeText_->SetTextColor(glm::vec3(0.4f, 0.44f, 0.5f));
        UI_MN.AddElement(displayModeText_);
    }

    void GameOverScene::update(double deltaTime)
    {
        blinkElapsed_ += deltaTime;

        if (promptText_)
        {
            promptText_->SetVisible(std::fmod(blinkElapsed_, 1.2) < 0.8);
        }

        if (displayModeText_)
        {
            displayModeText_->SetText("F11: " + DescribeDisplayMode());
        }

        HandleDisplayToggles();

        // Retry straight into a new run -- an arcade score-attack loop should not
        // force a detour through the menu after every death.
        if (INP_MN.IsActionPressed(kActionConfirm, CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new GameplayScene());
            return;
        }

        if (INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new MainMenuScene());
        }
    }
}
