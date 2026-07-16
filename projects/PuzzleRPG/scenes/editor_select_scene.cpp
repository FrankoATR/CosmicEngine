#include "editor_select_scene.hpp"

#include "editor_scene.hpp"
#include "main_menu_scene.hpp"

#include "../systems/level_data.hpp"
#include "../utilities/puzzle_asset_defines.hpp"
#include "../utilities/puzzle_menu_music.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include "../systems/menu_background.hpp"

namespace PuzzleRPG
{
    EditorSelectScene::EditorSelectScene()
        : Scene("EditorSelectScene"),
          title(nullptr), statusLabel(nullptr),
          newLevelName(nullptr), createButton(nullptr), backButton(nullptr)
    {
    }

    void EditorSelectScene::loadResources()
    {
        RS_MN.LoadFont(PUZZLERPG_FONT_MAIN_KEY, PUZZLERPG_FONT_MAIN_PATH, PUZZLERPG_FONT_MAIN_SIZE);
        RS_MN.LoadFont(PUZZLERPG_FONT_TITLE_KEY, PUZZLERPG_FONT_TITLE_PATH, PUZZLERPG_FONT_TITLE_SIZE);
        AUD_MN.Load(PUZZLERPG_MUSIC_MENU_KEY, PUZZLERPG_MUSIC_MENU_PATH, CosmicEngine::SoundType::Music);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_KEY, PUZZLERPG_UI_BUTTON_IDLE_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_KEY, PUZZLERPG_UI_BUTTON_HOVER_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_M_KEY, PUZZLERPG_UI_BUTTON_IDLE_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_M_KEY, PUZZLERPG_UI_BUTTON_HOVER_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_L_KEY, PUZZLERPG_UI_BUTTON_IDLE_L_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_L_KEY, PUZZLERPG_UI_BUTTON_HOVER_L_PATH);
        PuzzleRPG::MenuBackground::Get().LoadResources();
    }

    void EditorSelectScene::init()
    {
        StartMenuMusic();
        PuzzleRPG::MenuBackground::Get().Init();

        title = new CosmicEngine::UIText("Editor de niveles", "puzzle_font_big",
                                         glm::vec2(660.0f, 30.0f),
                                         glm::vec2(600.0f, 100.0f), true);
        title->SetTextColor(glm::vec3(0.65f, 0.95f, 0.65f));
        UI_MN.AddElement(title);

        statusLabel = new CosmicEngine::UIText("", "puzzle_font",
                                               glm::vec2(660.0f, 940.0f),
                                               glm::vec2(600.0f, 50.0f), true);
        UI_MN.AddElement(statusLabel);

        // Listado de niveles existentes.
        const glm::vec2 buttonSize(420.0f, 70.0f);
        glm::vec2 pos(420.0f, 240.0f);
        auto levels = ListAvailableLevels();
        for (const auto &name : levels)
        {
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";

            auto *button = new CosmicEngine::UIButton(name, "puzzle_font", idleKey,
                                                      pos, buttonSize, true, true);
            button->SetHoverTexture(hoverKey);
            button->SetTextureScale(1.06f);
            button->SetTextOffset(glm::vec2(0.0f, -4.0f));
            std::string captured = name;
            button->SetOnClick([captured]()
            {
                PreserveMenuMusicForSceneChange();
                SCN_MN.ReplaceScene(new EditorScene(captured, false));
            });
            UI_MN.AddElement(button);
            levelButtons.push_back(button);
            pos.y += buttonSize.y + 16.0f;
        }

        // Crear nivel nuevo.
        newLevelName = new CosmicEngine::UITextField("nuevo_nivel", "Nombre del nivel",
                                                     "puzzle_font",
                                                     glm::vec2(1080.0f, 240.0f),
                                                     glm::vec2(420.0f, 70.0f),
                                                     32, true);
        UI_MN.AddElement(newLevelName);

        {
            std::string label = "Crear nivel nuevo";
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";
            createButton = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                      glm::vec2(1080.0f, 330.0f),
                                                      glm::vec2(420.0f, 70.0f), true, true);
            createButton->SetHoverTexture(hoverKey);
            createButton->SetTextureScale(1.06f);
            createButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
        }
        createButton->SetOnClick([this]()
        {
            PreserveMenuMusicForSceneChange();
            std::string name = newLevelName ? newLevelName->GetText() : "nuevo_nivel";
            if (name.empty()) name = "nuevo_nivel";
            SCN_MN.ReplaceScene(new EditorScene(name, true));
        });
        UI_MN.AddElement(createButton);

        {
            std::string label = "Volver";
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";
            backButton = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                    glm::vec2(80.0f, 940.0f),
                                                    glm::vec2(220.0f, 70.0f), true, true);
            backButton->SetHoverTexture(hoverKey);
            backButton->SetTextureScale(1.06f);
            backButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
            backButton->SetOnClick([]()
            {
                PreserveMenuMusicForSceneChange();
                SCN_MN.ReplaceScene(new MainMenuScene());
            });
            UI_MN.AddElement(backButton);
        }

        if (levels.empty())
        {
            statusLabel->SetText("No hay niveles. Crea uno con el panel de la derecha.");
        }
    }

    void EditorSelectScene::update(double deltaTime)
    {
        PuzzleRPG::MenuBackground::Get().Update(deltaTime);
    }
    void EditorSelectScene::draw()
    {
        PuzzleRPG::MenuBackground::Get().Draw();
    }
    void EditorSelectScene::reset() {}
}
