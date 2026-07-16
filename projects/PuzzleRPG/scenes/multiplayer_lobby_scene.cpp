#include "multiplayer_lobby_scene.hpp"

#include "main_menu_scene.hpp"
#include "level_select_scene.hpp"
#include "gameplay_scene.hpp"

#include "../systems/game_session.hpp"
#include "../utilities/puzzle_asset_defines.hpp"
#include "../utilities/puzzle_menu_music.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include NETWORKMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER

#include "../systems/menu_background.hpp"

#include <cstdlib>

namespace PuzzleRPG
{
    MultiplayerLobbyScene::MultiplayerLobbyScene()
        : Scene("MultiplayerLobbyScene"),
          title(nullptr), statusLabel(nullptr),
          ipField(nullptr), portField(nullptr), nameField(nullptr),
          hostButton(nullptr), joinButton(nullptr), backButton(nullptr)
    {
    }

    void MultiplayerLobbyScene::loadResources()
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

    void MultiplayerLobbyScene::init()
    {
        StartMenuMusic();
        PuzzleRPG::MenuBackground::Get().Init();

        title = new CosmicEngine::UIText("Multijugador", "puzzle_font_big",
                                         glm::vec2(680.0f, 30.0f),
                                         glm::vec2(560.0f, 100.0f), true);
        title->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
        UI_MN.AddElement(title);

        nameField = new CosmicEngine::UITextField(GameSession::Get().username, "Nombre",
                                                  "puzzle_font",
                                                  glm::vec2(680.0f, 280.0f),
                                                  glm::vec2(560.0f, 70.0f),
                                                  24, true);
        UI_MN.AddElement(nameField);

        ipField = new CosmicEngine::UITextField(GameSession::Get().hostIp, "IP",
                                                "puzzle_font",
                                                glm::vec2(680.0f, 380.0f),
                                                glm::vec2(380.0f, 70.0f),
                                                40, true);
        UI_MN.AddElement(ipField);

        portField = new CosmicEngine::UITextField(std::to_string(GameSession::Get().port), "Puerto",
                                                  "puzzle_font",
                                                  glm::vec2(1080.0f, 380.0f),
                                                  glm::vec2(160.0f, 70.0f),
                                                  5, true);
        UI_MN.AddElement(portField);

        {
            std::string label = "Crear partida (Host)";
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";
            hostButton = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                    glm::vec2(680.0f, 500.0f),
                                                    glm::vec2(560.0f, 80.0f), true, true);
            hostButton->SetHoverTexture(hoverKey);
        }
        hostButton->SetTextureScale(1.06f);
        hostButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
        UI_MN.AddElement(hostButton);
        hostButton->SetOnClick([this]()
        {
            PreserveMenuMusicForSceneChange();
            auto &session = GameSession::Get();
            session.mode     = GameSession::NetworkMode::Host;
            session.username = nameField ? nameField->GetText() : session.username;
            try { session.port = std::stoi(portField->GetText()); } catch (...) { session.port = 25566; }
            if (statusLabel) statusLabel->SetText("Iniciando servidor en " + std::to_string(session.port) + "...");
            SCN_MN.ReplaceScene(new LevelSelectScene(true));
        });

        {
            std::string label = "Unirse por IP";
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";
            joinButton = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                    glm::vec2(680.0f, 600.0f),
                                                    glm::vec2(560.0f, 80.0f), true, true);
            joinButton->SetHoverTexture(hoverKey);
        }
        joinButton->SetTextureScale(1.06f);
        joinButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
        UI_MN.AddElement(joinButton);
        joinButton->SetOnClick([this]()
        {
            PreserveMenuMusicForSceneChange();
            auto &session = GameSession::Get();
            session.mode     = GameSession::NetworkMode::Client;
            session.username = nameField ? nameField->GetText() : session.username;
            session.hostIp   = ipField ? ipField->GetText() : session.hostIp;
            try { session.port = std::stoi(portField->GetText()); } catch (...) { session.port = 25566; }
            if (statusLabel) statusLabel->SetText("Conectando a " + session.hostIp + ":" + std::to_string(session.port) + "...");
            if (NET_MN.ConnectToServer(session.hostIp, session.port, session.username))
            {
                // El host enviara el nivel al cliente; usamos el selectedLevel
                // recibido por el handler de Msg_LevelLoad en GameplayScene.
                SCN_MN.ReplaceScene(new GameplayScene());
            }
            else if (statusLabel)
            {
                statusLabel->SetText("No se pudo conectar a " + session.hostIp + ":" + std::to_string(session.port));
            }
        });

        {
            std::string label = "Volver";
            std::string idleKey = "ui/button-idle-m";
            std::string hoverKey = "ui/button-hover-m";
            backButton = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                    glm::vec2(80.0f, 940.0f),
                                                    glm::vec2(220.0f, 70.0f), true, true);
            backButton->SetHoverTexture(hoverKey);
        }
        backButton->SetTextureScale(1.06f);
        backButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
        UI_MN.AddElement(backButton);
        backButton->SetOnClick([]()
        {
            PreserveMenuMusicForSceneChange();
            SCN_MN.ReplaceScene(new MainMenuScene());
        });

        statusLabel = new CosmicEngine::UIText("", "puzzle_font",
                                               glm::vec2(680.0f, 720.0f),
                                               glm::vec2(560.0f, 60.0f), true);
        statusLabel->SetTextColor(glm::vec3(0.9f, 0.9f, 0.5f));
        UI_MN.AddElement(statusLabel);
    }

    void MultiplayerLobbyScene::update(double deltaTime)
    {
        PuzzleRPG::MenuBackground::Get().Update(deltaTime);
    }
    void MultiplayerLobbyScene::draw()
    {
        PuzzleRPG::MenuBackground::Get().Draw();
    }
    void MultiplayerLobbyScene::reset() {}
}
