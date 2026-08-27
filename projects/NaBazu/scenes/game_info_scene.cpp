#include "game_info_scene.hpp"

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
    constexpr float kColumnLeftX = 150.0f;
    constexpr float kColumnRightX = 1010.0f;
    constexpr float kColumnWidth = 780.0f;

    constexpr float kHeadingScale = 1.15f;
    constexpr float kLineScale = 0.92f;
    constexpr float kLineHeight = 46.0f;
    constexpr float kSectionGap = 78.0f;
}

namespace NaBazu
{
    GameInfoScene::GameInfoScene()
        : Scene("GameInfoScene"),
          displayModeText_(nullptr)
    {
    }

    void GameInfoScene::loadResources()
    {
        RS_MN.LoadFont(NABAZU_FONT_MAIN_KEY, NABAZU_FONT_MAIN_PATH, 32);
    }

    void GameInfoScene::AddHeading(const std::string &text, float x, float y)
    {
        auto *heading = new CosmicEngine::UIText(text, NABAZU_FONT_MAIN_KEY, glm::vec2(x, y),
                                                  glm::vec2(kColumnWidth, 44.0f), true, nullptr, false, 1, true);
        heading->SetTextScale(kHeadingScale);
        heading->SetTextColor(glm::vec3(0.35f, 0.8f, 1.0f));
        UI_MN.AddElement(heading);
    }

    void GameInfoScene::AddLine(const std::string &text, float x, float y, glm::vec3 color)
    {
        auto *line = new CosmicEngine::UIText(text, NABAZU_FONT_MAIN_KEY, glm::vec2(x, y),
                                               glm::vec2(kColumnWidth, 40.0f), true, nullptr, false, 1, true);
        line->SetTextScale(kLineScale);
        line->SetTextColor(color);
        UI_MN.AddElement(line);
    }

    void GameInfoScene::init()
    {
        RegisterNaBazuInputActions();

        SCN_MN.SetBackgroundColor(glm::vec3(0.015f, 0.02f, 0.04f));

        auto *title = new CosmicEngine::UIText("INFORMACION DE JUEGO", NABAZU_FONT_MAIN_KEY,
            glm::vec2(0.0f, 90.0f), glm::vec2(1920.0f, 90.0f), true);
        title->SetTextScale(1.7f);
        title->SetTextColor(glm::vec3(0.25f, 0.85f, 1.0f));
        UI_MN.AddElement(title);

        const glm::vec3 body(0.72f, 0.76f, 0.82f);

        // --- Left column: controls ------------------------------------------------
        float y = 240.0f;
        AddHeading("CONTROLES", kColumnLeftX, y);
        y += kLineHeight * 1.3f;

        const char *controls[] = {
            "Flechas / WASD .... Mover la nave",
            "Espacio ........... Disparo rapido",
            "Shift izq. ........ Misil de area",
            "Ctrl / C .......... Frenar (2s, luego enfria)",
            "F11 ............... Pantalla completa",
            "B ................. Ver hitboxes (debug)",
            "F3 ................ Modo inspeccion",
            "Esc ............... Salir"
        };
        for (const char *line : controls)
        {
            AddLine(line, kColumnLeftX, y, body);
            y += kLineHeight;
        }

        y += kSectionGap * 0.4f;
        AddHeading("MODO INSPECCION (F3)", kColumnLeftX, y);
        y += kLineHeight * 1.3f;
        AddLine("Mouse ............. Orbitar la camara", kColumnLeftX, y, body);
        y += kLineHeight;
        AddLine("Rueda ............. Acercar / alejar", kColumnLeftX, y, body);
        y += kLineHeight;
        AddLine("J / K ............. Avanzar / retroceder", kColumnLeftX, y, body);
        y += kLineHeight;
        AddLine("O ................. Congelar entidades", kColumnLeftX, y, body);

        // --- Right column: pickups and tips ---------------------------------------
        y = 240.0f;
        AddHeading("OBJETOS", kColumnRightX, y);
        y += kLineHeight * 1.3f;
        AddLine("Estrella de 3 puntas ... +30 municion", kColumnRightX, y, glm::vec3(0.35f, 0.9f, 0.45f));
        y += kLineHeight;
        AddLine("Estrella de 5 puntas ... +2 misiles", kColumnRightX, y, glm::vec3(0.8f, 0.45f, 1.0f));
        y += kLineHeight;
        AddLine("Anillo rojo ............ +25% integridad", kColumnRightX, y, glm::vec3(1.0f, 0.35f, 0.35f));

        y += kSectionGap;
        AddHeading("CONSEJOS", kColumnRightX, y);
        y += kLineHeight * 1.3f;
        AddLine("La mira se encoge al fijar un objetivo.", kColumnRightX, y, body);
        y += kLineHeight;
        AddLine("Frenar da margen para esquivar oleadas.", kColumnRightX, y, body);
        y += kLineHeight;
        AddLine("El misil dana en area: agrupa enemigos.", kColumnRightX, y, body);
        y += kLineHeight;
        AddLine("Los enemigos sueltan objetos al morir.", kColumnRightX, y, body);
        y += kLineHeight;
        AddLine("El nivel es infinito: sobrevive y suma.", kColumnRightX, y, body);

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

    void GameInfoScene::update(double deltaTime)
    {
        (void)deltaTime;

        if (displayModeText_)
        {
            displayModeText_->SetText("F11: " + DescribeDisplayMode());
        }

        HandleDisplayToggles();

        if (INP_MN.IsActionPressed(kActionConfirm, CosmicEngine::KeyDown) ||
            INP_MN.IsActionPressed("system_exit_game", CosmicEngine::KeyDown))
        {
            SCN_MN.ReplaceScene(new MainMenuScene());
        }
    }
}
