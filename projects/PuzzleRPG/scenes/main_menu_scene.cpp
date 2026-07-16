#include "main_menu_scene.hpp"

#include "editor_scene.hpp"
#include "gameplay_scene.hpp"

#include "../systems/game_session.hpp"
#include "../systems/level_data.hpp"
#include "../utilities/puzzle_asset_defines.hpp"
#include "../utilities/puzzle_input_actions.hpp"
#include "../utilities/puzzle_menu_music.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <cosmic_project_config.hpp>
#include AUDIOMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include RESOURCEMANAGER_HEADER
#include NETWORKMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER
#include <CosmicEngine/managers/input/input_manager.hpp>
#include "../systems/menu_background.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace PuzzleRPG
{
    namespace
    {
        constexpr glm::vec2 kButtonSize(420.0f, 80.0f);
        constexpr float     kButtonGap = 24.0f;
        constexpr glm::vec2 kFirstButton(750.0f, 400.0f);
        constexpr glm::vec2 kPanelButtonSize(420.0f, 70.0f);
        constexpr float     kPanelButtonGap = 16.0f;

        struct RuntimeSettingsData
        {
            float masterVolume = 1.0f;
            float musicVolume = 1.0f;
            float sfxVolume = 1.0f;
            bool fullscreen = false;
            bool vsync = false;
        };

        std::filesystem::path GetRuntimeSettingsPath()
        {
            return std::filesystem::current_path()
                / "config"
                / (std::string(CosmicEngine::ProjectConfig::kProjectName) + "_settings.json");
        }

        std::string Trim(std::string text)
        {
            const auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
            text.erase(text.begin(), std::find_if(text.begin(), text.end(), notSpace));
            text.erase(std::find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
            return text;
        }

        std::string ToUpper(std::string text)
        {
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
            {
                return static_cast<char>(std::toupper(ch));
            });
            return text;
        }

        std::vector<std::string> SplitCsv(const std::string &text)
        {
            std::vector<std::string> tokens;
            std::stringstream stream(text);
            std::string token;
            while (std::getline(stream, token, ','))
            {
                token = Trim(token);
                if (!token.empty()) tokens.push_back(token);
            }
            return tokens;
        }

        int KeyboardCodeFromName(const std::string &name)
        {
            const std::string upper = ToUpper(Trim(name));
            if (upper.empty()) return -1;
            if (upper == "UP") return GLFW_KEY_UP;
            if (upper == "DOWN") return GLFW_KEY_DOWN;
            if (upper == "LEFT") return GLFW_KEY_LEFT;
            if (upper == "RIGHT") return GLFW_KEY_RIGHT;
            if (upper == "SPACE") return GLFW_KEY_SPACE;
            if (upper == "ENTER" || upper == "RETURN") return GLFW_KEY_ENTER;
            if (upper == "ESC" || upper == "ESCAPE") return GLFW_KEY_ESCAPE;
            if (upper == "TAB") return GLFW_KEY_TAB;
            if (upper == "LSHIFT" || upper == "LEFTSHIFT") return GLFW_KEY_LEFT_SHIFT;
            if (upper == "RSHIFT" || upper == "RIGHTSHIFT") return GLFW_KEY_RIGHT_SHIFT;
            if (upper == "PGUP" || upper == "PAGEUP") return GLFW_KEY_PAGE_UP;
            if (upper == "PGDOWN" || upper == "PAGEDOWN") return GLFW_KEY_PAGE_DOWN;
            if (upper.size() == 1)
            {
                const char ch = upper[0];
                if (ch >= 'A' && ch <= 'Z') return GLFW_KEY_A + (ch - 'A');
                if (ch >= '0' && ch <= '9') return GLFW_KEY_0 + (ch - '0');
            }
            try { return std::stoi(upper); }
            catch (...) { return -1; }
        }

        std::string KeyboardNameFromCode(int code)
        {
            switch (code)
            {
                case GLFW_KEY_UP: return "Up";
                case GLFW_KEY_DOWN: return "Down";
                case GLFW_KEY_LEFT: return "Left";
                case GLFW_KEY_RIGHT: return "Right";
                case GLFW_KEY_SPACE: return "Space";
                case GLFW_KEY_ENTER: return "Enter";
                case GLFW_KEY_ESCAPE: return "Escape";
                case GLFW_KEY_TAB: return "Tab";
                case GLFW_KEY_LEFT_SHIFT: return "LShift";
                case GLFW_KEY_RIGHT_SHIFT: return "RShift";
                case GLFW_KEY_PAGE_UP: return "PageUp";
                case GLFW_KEY_PAGE_DOWN: return "PageDown";
                default:
                    if (code >= GLFW_KEY_A && code <= GLFW_KEY_Z)
                    {
                        return std::string(1, static_cast<char>('A' + (code - GLFW_KEY_A)));
                    }
                    if (code >= GLFW_KEY_0 && code <= GLFW_KEY_9)
                    {
                        return std::string(1, static_cast<char>('0' + (code - GLFW_KEY_0)));
                    }
                    return std::to_string(code);
            }
        }

        int GamepadButtonFromName(const std::string &name)
        {
            const std::string upper = ToUpper(Trim(name));
            if (upper.empty()) return -1;
            if (upper == "A") return GLFW_GAMEPAD_BUTTON_A;
            if (upper == "B") return GLFW_GAMEPAD_BUTTON_B;
            if (upper == "X") return GLFW_GAMEPAD_BUTTON_X;
            if (upper == "Y") return GLFW_GAMEPAD_BUTTON_Y;
            if (upper == "LB" || upper == "L1") return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
            if (upper == "RB" || upper == "R1") return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
            if (upper == "BACK" || upper == "SELECT") return GLFW_GAMEPAD_BUTTON_BACK;
            if (upper == "START") return GLFW_GAMEPAD_BUTTON_START;
            if (upper == "GUIDE") return GLFW_GAMEPAD_BUTTON_GUIDE;
            if (upper == "LS" || upper == "L3") return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
            if (upper == "RS" || upper == "R3") return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
            if (upper == "DPADUP" || upper == "DUP") return GLFW_GAMEPAD_BUTTON_DPAD_UP;
            if (upper == "DPADDOWN" || upper == "DDOWN") return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
            if (upper == "DPADLEFT" || upper == "DLEFT") return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
            if (upper == "DPADRIGHT" || upper == "DRIGHT") return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
            try { return std::stoi(upper); }
            catch (...) { return -1; }
        }

        std::string GamepadNameFromButton(int code)
        {
            switch (code)
            {
                case GLFW_GAMEPAD_BUTTON_A: return "A";
                case GLFW_GAMEPAD_BUTTON_B: return "B";
                case GLFW_GAMEPAD_BUTTON_X: return "X";
                case GLFW_GAMEPAD_BUTTON_Y: return "Y";
                case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return "LB";
                case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return "RB";
                case GLFW_GAMEPAD_BUTTON_BACK: return "Back";
                case GLFW_GAMEPAD_BUTTON_START: return "Start";
                case GLFW_GAMEPAD_BUTTON_GUIDE: return "Guide";
                case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return "LS";
                case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return "RS";
                case GLFW_GAMEPAD_BUTTON_DPAD_UP: return "DPadUp";
                case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return "DPadDown";
                case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return "DPadLeft";
                case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return "DPadRight";
                default: return std::to_string(code);
            }
        }

        std::string JoinKeyboardCodes(const std::vector<int> &codes)
        {
            std::string result;
            for (std::size_t i = 0; i < codes.size(); ++i)
            {
                if (i > 0) result += ", ";
                result += KeyboardNameFromCode(codes[i]);
            }
            return result;
        }

        std::string JoinGamepadButtons(const std::vector<int> &codes)
        {
            std::string result;
            for (std::size_t i = 0; i < codes.size(); ++i)
            {
                if (i > 0) result += ", ";
                result += GamepadNameFromButton(codes[i]);
            }
            return result;
        }

        std::vector<int> ParseKeyboardCodes(const std::string &text)
        {
            std::vector<int> codes;
            for (const auto &token : SplitCsv(text))
            {
                const int code = KeyboardCodeFromName(token);
                if (code >= 0) codes.push_back(code);
            }
            return codes;
        }

        std::vector<int> ParseGamepadButtons(const std::string &text)
        {
            std::vector<int> codes;
            for (const auto &token : SplitCsv(text))
            {
                const int code = GamepadButtonFromName(token);
                if (code >= 0) codes.push_back(code);
            }
            return codes;
        }

        void ApplyWindowSettings(bool fullscreen, bool vsync)
        {
            if (GM_MN.isFullScreen() != fullscreen)
            {
                GM_MN.toggleFullscreen();
            }

            if (vsync) GM_MN.enableVsync();
            else       GM_MN.disableVsync();
        }

        RuntimeSettingsData LoadRuntimeSettings()
        {
            RuntimeSettingsData settings;
            std::ifstream input(GetRuntimeSettingsPath(), std::ios::binary);
            if (!input.is_open())
            {
                settings.fullscreen = GM_MN.isFullScreen();
                settings.vsync = GM_MN.isVsyncEnabled();
                return settings;
            }

            try
            {
                nlohmann::json document = nlohmann::json::parse(input);
                settings.masterVolume = std::clamp(document.value("masterVolume", settings.masterVolume), 0.0f, 1.0f);
                settings.musicVolume = std::clamp(document.value("musicVolume", settings.musicVolume), 0.0f, 1.0f);
                settings.sfxVolume = std::clamp(document.value("sfxVolume", settings.sfxVolume), 0.0f, 1.0f);
                settings.fullscreen = document.value("fullscreen", GM_MN.isFullScreen());
                settings.vsync = document.value("vsync", GM_MN.isVsyncEnabled());
            }
            catch (...)
            {
                settings.fullscreen = GM_MN.isFullScreen();
                settings.vsync = GM_MN.isVsyncEnabled();
            }

            return settings;
        }

        void SaveRuntimeSettings(const RuntimeSettingsData &settings)
        {
            const auto path = GetRuntimeSettingsPath();
            std::filesystem::create_directories(path.parent_path());
            nlohmann::json document = {
                {"version", 1},
                {"masterVolume", settings.masterVolume},
                {"musicVolume", settings.musicVolume},
                {"sfxVolume", settings.sfxVolume},
                {"fullscreen", settings.fullscreen},
                {"vsync", settings.vsync}
            };

            std::ofstream output(path, std::ios::binary | std::ios::trunc);
            if (output.is_open())
            {
                output << document.dump(2);
            }
        }
    }

    MainMenuScene::MainMenuScene()
        : Scene("MainMenuScene"),
          title(nullptr),
          statusLabel(nullptr),
          backButton(nullptr),
          nameField(nullptr),
          ipField(nullptr),
          portField(nullptr),
          newLevelName(nullptr),
          hostButton(nullptr),
          joinButton(nullptr),
          createLevelButton(nullptr),
            masterVolumeSlider(nullptr),
            musicVolumeSlider(nullptr),
            sfxVolumeSlider(nullptr),
            deadzoneSlider(nullptr),
            fullscreenButton(nullptr),
            vsyncButton(nullptr),
            applySettingsButton(nullptr),
            captureHintLabel(nullptr),
          creditsButton(nullptr),
          versionLabel(nullptr),
          currentPanel(MenuPanel::Root),
            resourcesLoaded(false),
            settingsFullscreen(false),
            settingsVsync(false),
            captureIndex(-1),
            captureForGamepad(false),
            captureArmFrames(0)
    {
    }

    void MainMenuScene::loadResources()
    {
        if (resourcesLoaded) return;
        RS_MN.LoadFont(PUZZLERPG_FONT_MAIN_KEY, PUZZLERPG_FONT_MAIN_PATH, PUZZLERPG_FONT_MAIN_SIZE);
        RS_MN.LoadFont(PUZZLERPG_FONT_TITLE_KEY, PUZZLERPG_FONT_TITLE_PATH, PUZZLERPG_FONT_TITLE_SIZE);
        // PuzzleRPG UI textures
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_KEY, PUZZLERPG_UI_BUTTON_IDLE_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_KEY, PUZZLERPG_UI_BUTTON_HOVER_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_M_KEY, PUZZLERPG_UI_BUTTON_IDLE_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_M_KEY, PUZZLERPG_UI_BUTTON_HOVER_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_L_KEY, PUZZLERPG_UI_BUTTON_IDLE_L_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_L_KEY, PUZZLERPG_UI_BUTTON_HOVER_L_PATH);
        // logo for main menu title
        RS_MN.LoadTexture(PUZZLERPG_UI_LOGO_KEY, PUZZLERPG_UI_LOGO_PATH);
        // menu background resource
        PuzzleRPG::MenuBackground::Get().LoadResources();
        resourcesLoaded = true;
    }

    CosmicEngine::UIButton *MainMenuScene::CreateMenuButton(const std::string &label,
                                                            glm::vec2 position,
                                                            glm::vec2 size,
                                                            std::function<void()> onClick)
    {
        std::string idleKey = "ui/button-idle-m";
        std::string hoverKey = "ui/button-hover-m";

        auto *button = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                  position, size, true, true);
        // apply PuzzleRPG button styling
        button->SetHoverTexture(hoverKey);
        button->SetTextureScale(1.06f);
        button->SetTextOffset(glm::vec2(0.0f, -4.0f));
        button->SetTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
        button->SetSelectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f));
        button->SetOnClick(std::move(onClick));
        UI_MN.AddElement(button);
        return button;
    }

    void MainMenuScene::SetGroupVisible(const std::vector<CosmicEngine::UIElement *> &elements, bool visible)
    {
        for (auto *element : elements)
        {
            if (element) element->SetVisible(visible);
        }
    }

    void MainMenuScene::BuildRootButtons()
    {
        struct Option { const char *label; std::function<void()> action; };
        std::vector<Option> options =
        {
            { "Jugar (1 jugador)", [this]()
                {
                    auto &session = GameSession::Get();
                    session.mode = GameSession::NetworkMode::SinglePlayer;
                    SetPanel(MenuPanel::SinglePlayerLevels);
                }
            },
            { "Multijugador",      [this]() { SetPanel(MenuPanel::Multiplayer); } },
            { "Editor de niveles", [this]() { SetPanel(MenuPanel::EditorLevels); } },
            { "Configuracion",     [this]() { SetPanel(MenuPanel::Settings); } },
            { "Salir",             []() { GM_MN.endprogram(); } }
        };

        glm::vec2 pos = kFirstButton;
        for (auto &option : options)
        {
            auto *button = CreateMenuButton(option.label, pos, kButtonSize, option.action);
            rootButtons.push_back(button);
            pos.y += kButtonSize.y + kButtonGap;
        }
    }

    void MainMenuScene::BuildLevelButtons()
    {
        auto levels = ListAvailableLevels();
        glm::vec2 pos(750.0f, 240.0f);
        for (const auto &levelName : levels)
        {
            std::string captured = levelName;
            auto *singleButton = CreateMenuButton(captured, pos, kPanelButtonSize,
                [captured]()
                {
                    auto &session = GameSession::Get();
                    session.mode = GameSession::NetworkMode::SinglePlayer;
                    session.selectedLevel = captured;
                    SCN_MN.ReplaceScene(new GameplayScene());
                });
            singleButton->SetVisible(false);
            singleLevelButtons.push_back(singleButton);

            auto *hostButtonLocal = CreateMenuButton(captured, pos, kPanelButtonSize,
                [captured]()
                {
                    auto &session = GameSession::Get();
                    session.mode = GameSession::NetworkMode::Host;
                    session.selectedLevel = captured;
                    NET_MN.StartServer(session.port);
                    SCN_MN.ReplaceScene(new GameplayScene());
                });
            hostButtonLocal->SetVisible(false);
            hostLevelButtons.push_back(hostButtonLocal);

            pos.y += kPanelButtonSize.y + kPanelButtonGap;
        }

        pos = glm::vec2(420.0f, 240.0f);
        for (const auto &levelName : levels)
        {
            std::string captured = levelName;
            auto *editorButton = CreateMenuButton(captured, pos, kPanelButtonSize,
                [captured]()
                {
                    SCN_MN.ReplaceScene(new EditorScene(captured, false));
                });
            editorButton->SetVisible(false);
            editorLevelButtons.push_back(editorButton);
            pos.y += kPanelButtonSize.y + kPanelButtonGap;
        }
    }

    void MainMenuScene::SetPanel(MenuPanel panel)
    {
        currentPanel = panel;

        if (logoImage) logoImage->SetVisible(panel == MenuPanel::Root);

        SetGroupVisible({ title, statusLabel, backButton }, true);
        SetGroupVisible({
            nameField, ipField, portField, hostButton, joinButton,
            newLevelName, createLevelButton
        }, false);
        SetGroupVisible(settingsElements, false);
        SetGroupVisible(creditsElements, false);
        if (creditsButton) creditsButton->SetVisible(panel == MenuPanel::Root);
        if (versionLabel) versionLabel->SetVisible(panel == MenuPanel::Root);

        for (auto *button : rootButtons) button->SetVisible(false);
        for (auto *button : singleLevelButtons) button->SetVisible(false);
        for (auto *button : hostLevelButtons) button->SetVisible(false);
        for (auto *button : editorLevelButtons) button->SetVisible(false);

        backButton->SetVisible(panel != MenuPanel::Root);
        statusLabel->SetText("");

        switch (panel)
        {
                case MenuPanel::Root:
                // Root uses the logo image instead of the UIText title.
                title->SetVisible(false);
                for (auto *button : rootButtons) button->SetVisible(true);
                break;

            case MenuPanel::SinglePlayerLevels:
                title->SetText("Seleccionar nivel");
                title->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                for (auto *button : singleLevelButtons) button->SetVisible(true);
                if (singleLevelButtons.empty())
                    statusLabel->SetText(std::string("No hay niveles en ") + PUZZLERPG_LEVELS_DIR + "/.");
                break;

            case MenuPanel::Multiplayer:
                title->SetText("Multijugador");
                title->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                SetGroupVisible(multiplayerElements, true);
                break;

            case MenuPanel::HostLevelSelect:
                title->SetText("Seleccionar nivel");
                title->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                for (auto *button : hostLevelButtons) button->SetVisible(true);
                if (hostLevelButtons.empty())
                    statusLabel->SetText("No hay niveles para hospedar.");
                break;

            case MenuPanel::EditorLevels:
                title->SetText("Editor de niveles");
                title->SetTextColor(glm::vec3(0.65f, 0.95f, 0.65f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                for (auto *button : editorLevelButtons) button->SetVisible(true);
                SetGroupVisible(editorCreateElements, true);
                if (editorLevelButtons.empty())
                    statusLabel->SetText("No hay niveles. Crea uno con el panel derecho.");
                break;

            case MenuPanel::Settings:
                title->SetText("Configuracion");
                title->SetTextColor(glm::vec3(0.80f, 0.90f, 1.00f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                RefreshSettingsFields();
                SetGroupVisible(settingsElements, true);
                break;

            case MenuPanel::Credits:
                title->SetText("Creditos");
                title->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
                title->SetFont("puzzle_font_big");
                title->SetPosition(glm::vec2(660.0f, 60.0f));
                SetGroupVisible(creditsElements, true);
                break;
        }
    }

    void MainMenuScene::BuildSettingsPanel()
    {
        auto createLabel = [this](const std::string &text, glm::vec2 position, glm::vec2 size, glm::vec3 color)
        {
            auto *label = new CosmicEngine::UIText(text, "puzzle_font", position, size, false);
            label->SetTextColor(color);
            UI_MN.AddElement(label);
            settingsElements.push_back(label);
            return label;
        };

        auto createSlider = [this](glm::vec2 position, glm::vec2 size, float minV, float maxV, float initial)
        {
            auto *slider = new CosmicEngine::UISlider("", "puzzle_font", position, size, minV, maxV, initial, false);
            UI_MN.AddElement(slider);
            settingsElements.push_back(slider);
            return slider;
        };

        const glm::vec2 sliderSize(300.0f, 36.0f);
        const glm::vec2 labelSize(190.0f, 32.0f);
        const glm::vec3 sectionColor(0.95f, 0.85f, 0.30f);
        const glm::vec3 rowColor(0.85f, 0.95f, 0.95f);

        auto percentFormatter = [](float v) {
            return std::to_string(static_cast<int>(v * 100.0f + 0.5f)) + "%";
        };
        auto deadzoneFormatter = [](float v) {
            std::ostringstream s; s.setf(std::ios::fixed); s.precision(2); s << v; return s.str();
        };

        createLabel("Audio", glm::vec2(280.0f, 220.0f), glm::vec2(320.0f, 36.0f), sectionColor);

        createLabel("Master", glm::vec2(280.0f, 282.0f), labelSize, rowColor);
        masterVolumeSlider = createSlider(glm::vec2(480.0f, 280.0f), sliderSize, 0.0f, 1.0f, AUD_MN.GetMasterVolume());
        masterVolumeSlider->SetValueFormatter(percentFormatter);
        masterVolumeSlider->SetOnValueChanged([](float v) { AUD_MN.SetMasterVolume(v); });

        createLabel("Musica", glm::vec2(280.0f, 342.0f), labelSize, rowColor);
        musicVolumeSlider = createSlider(glm::vec2(480.0f, 340.0f), sliderSize, 0.0f, 1.0f, AUD_MN.GetMusicMasterVolume());
        musicVolumeSlider->SetValueFormatter(percentFormatter);
        musicVolumeSlider->SetOnValueChanged([](float v) { AUD_MN.SetMusicMasterVolume(v); });

        createLabel("SFX", glm::vec2(280.0f, 402.0f), labelSize, rowColor);
        sfxVolumeSlider = createSlider(glm::vec2(480.0f, 400.0f), sliderSize, 0.0f, 1.0f, AUD_MN.GetSfxMasterVolume());
        sfxVolumeSlider->SetValueFormatter(percentFormatter);
        sfxVolumeSlider->SetOnValueChanged([](float v) { AUD_MN.SetSfxMasterVolume(v); });

        createLabel("Sistema", glm::vec2(280.0f, 470.0f), glm::vec2(320.0f, 36.0f), sectionColor);

        createLabel("Deadzone", glm::vec2(280.0f, 532.0f), labelSize, rowColor);
        deadzoneSlider = createSlider(glm::vec2(480.0f, 530.0f), sliderSize, 0.0f, 0.99f, INP_MN.GetGamepadDeadzone());
        deadzoneSlider->SetValueFormatter(deadzoneFormatter);
        deadzoneSlider->SetOnValueChanged([](float v) { INP_MN.SetGamepadDeadzone(v); });

        fullscreenButton = CreateMenuButton("Fullscreen: OFF", glm::vec2(280.0f, 590.0f), glm::vec2(360.0f, 58.0f),
            [this]()
            {
                settingsFullscreen = !settingsFullscreen;
                ApplyWindowSettings(settingsFullscreen, settingsVsync);
                UpdateSettingsToggleLabels();
            });
        settingsElements.push_back(fullscreenButton);
        fullscreenButton->SetVisible(false);

        vsyncButton = CreateMenuButton("VSync: OFF", glm::vec2(280.0f, 658.0f), glm::vec2(360.0f, 58.0f),
            [this]()
            {
                settingsVsync = !settingsVsync;
                ApplyWindowSettings(settingsFullscreen, settingsVsync);
                UpdateSettingsToggleLabels();
            });
        settingsElements.push_back(vsyncButton);
        vsyncButton->SetVisible(false);

        // Center the main controls title and align the column headers
        createLabel("Mapeo de controles", glm::vec2(1260.0f, 220.0f), glm::vec2(420.0f, 36.0f), sectionColor);
        // Place headers directly above their columns: actions, keyboard, gamepad
        createLabel("Accion", glm::vec2(910.0f, 262.0f), glm::vec2(220.0f, 24.0f), glm::vec3(0.65f, 0.90f, 1.0f));
        createLabel("Teclado", glm::vec2(1140.0f, 262.0f), glm::vec2(200.0f, 24.0f), glm::vec3(0.65f, 0.90f, 1.0f));
        createLabel("Mando", glm::vec2(1620.0f, 262.0f), glm::vec2(200.0f, 24.0f), glm::vec3(0.65f, 0.90f, 1.0f));

        float rowY = 304.0f;
        const auto &specs = GetPuzzleInputActionSpecs();
        for (std::size_t i = 0; i < specs.size(); ++i)
        {
            const auto &spec = specs[i];
            auto *actionLabel = createLabel(spec.displayName, glm::vec2(900.0f, rowY + 8.0f), glm::vec2(240.0f, 28.0f), rowColor);
            settingsInputLabels.push_back(actionLabel);

            std::size_t captureIdx = i;
            // Keyboard binding button: show currently mapped keys directly on the button
            const auto binding = INP_MN.GetActionBinding(spec.actionName);
            const std::string kbText = binding.keyboardKeys.empty() ? "-" : JoinKeyboardCodes(binding.keyboardKeys);
            auto *kbButton = CreateMenuButton(kbText, glm::vec2(1140.0f, rowY), glm::vec2(200.0f, 44.0f),
                [this, captureIdx]() { StartCapture(captureIdx, false); });
            settingsElements.push_back(kbButton);
            settingsKeyboardCaptureButtons.push_back(kbButton);
            kbButton->SetVisible(true);

            const std::string padText = binding.gamepadButtons.empty() ? "-" : JoinGamepadButtons(binding.gamepadButtons);
            auto *padButton = CreateMenuButton(padText, glm::vec2(1620.0f, rowY), glm::vec2(200.0f, 44.0f),
                [this, captureIdx]() { StartCapture(captureIdx, true); });
            settingsElements.push_back(padButton);
            settingsGamepadCaptureButtons.push_back(padButton);
            padButton->SetVisible(true);

            rowY += 56.0f;
        }

        captureHintLabel = new CosmicEngine::UIText("", "puzzle_font",
            glm::vec2(900.0f, rowY + 16.0f), glm::vec2(820.0f, 32.0f), false);
        captureHintLabel->SetTextColor(glm::vec3(0.95f, 0.85f, 0.40f));
        UI_MN.AddElement(captureHintLabel);
        settingsElements.push_back(captureHintLabel);

        applySettingsButton = CreateMenuButton("Aplicar y guardar", glm::vec2(1200.0f, 800.0f), glm::vec2(420.0f, 72.0f),
            [this]() { ApplySettingsFromFields(); });
        settingsElements.push_back(applySettingsButton);
        applySettingsButton->SetVisible(false);
    }

    void MainMenuScene::UpdateBindingDisplay(std::size_t index)
    {
        const auto &specs = GetPuzzleInputActionSpecs();
        if (index >= specs.size()) return;
        const auto binding = INP_MN.GetActionBinding(specs[index].actionName);
        if (index < settingsKeyboardCaptureButtons.size())
        {
            const std::string text = binding.keyboardKeys.empty() ? "-" : JoinKeyboardCodes(binding.keyboardKeys);
            settingsKeyboardCaptureButtons[index]->SetText(text);
        }
        if (index < settingsGamepadCaptureButtons.size())
        {
            const std::string text = binding.gamepadButtons.empty() ? "-" : JoinGamepadButtons(binding.gamepadButtons);
            settingsGamepadCaptureButtons[index]->SetText(text);
        }
    }

    void MainMenuScene::StartCapture(std::size_t index, bool gamepad)
    {
        captureIndex = static_cast<int>(index);
        captureForGamepad = gamepad;
        captureArmFrames = 1;
        if (captureHintLabel)
        {
            captureHintLabel->SetText(gamepad
                ? "Pulsa un boton del mando... (Esc para cancelar)"
                : "Pulsa una tecla... (Esc para cancelar)");
        }
        if (statusLabel) statusLabel->SetText("");
    }

    void MainMenuScene::CancelCapture()
    {
        captureIndex = -1;
        captureArmFrames = 0;
        if (captureHintLabel) captureHintLabel->SetText("");
    }

    void MainMenuScene::ApplyCapturedKeyboard(std::size_t index, int keyCode)
    {
        const auto &specs = GetPuzzleInputActionSpecs();
        if (index >= specs.size()) return;
        auto binding = INP_MN.GetActionBinding(specs[index].actionName);
        binding.keyboardKeys = { keyCode };
        INP_MN.SetActionBinding(specs[index].actionName, binding);
        INP_MN.SaveBindingsToJson(true);
        UpdateBindingDisplay(index);
        if (statusLabel)
        {
            statusLabel->SetText("Tecla asignada: " + KeyboardNameFromCode(keyCode));
        }
    }

    void MainMenuScene::ApplyCapturedGamepad(std::size_t index, int buttonCode)
    {
        const auto &specs = GetPuzzleInputActionSpecs();
        if (index >= specs.size()) return;
        auto binding = INP_MN.GetActionBinding(specs[index].actionName);
        binding.gamepadButtons = { buttonCode };
        INP_MN.SetActionBinding(specs[index].actionName, binding);
        INP_MN.SaveBindingsToJson(true);
        UpdateBindingDisplay(index);
        if (statusLabel)
        {
            statusLabel->SetText("Boton asignado: " + GamepadNameFromButton(buttonCode));
        }
    }

    void MainMenuScene::RefreshSettingsFields()
    {
        settingsFullscreen = GM_MN.isFullScreen();
        settingsVsync = GM_MN.isVsyncEnabled();

        if (masterVolumeSlider) masterVolumeSlider->SetValue(AUD_MN.GetMasterVolume(), false);
        if (musicVolumeSlider)  musicVolumeSlider->SetValue(AUD_MN.GetMusicMasterVolume(), false);
        if (sfxVolumeSlider)    sfxVolumeSlider->SetValue(AUD_MN.GetSfxMasterVolume(), false);
        if (deadzoneSlider)     deadzoneSlider->SetValue(INP_MN.GetGamepadDeadzone(), false);

        const auto &specs = GetPuzzleInputActionSpecs();
        for (std::size_t index = 0; index < specs.size(); ++index)
        {
            UpdateBindingDisplay(index);
        }

        UpdateSettingsToggleLabels();
        CancelCapture();
    }

    void MainMenuScene::UpdateSettingsToggleLabels()
    {
        if (fullscreenButton) fullscreenButton->SetText(std::string("Fullscreen: ") + (settingsFullscreen ? "ON" : "OFF"));
        if (vsyncButton) vsyncButton->SetText(std::string("VSync: ") + (settingsVsync ? "ON" : "OFF"));
    }

    void MainMenuScene::ApplySettingsFromFields()
    {
        const float masterVolume = masterVolumeSlider ? masterVolumeSlider->GetValue() : AUD_MN.GetMasterVolume();
        const float musicVolume  = musicVolumeSlider  ? musicVolumeSlider->GetValue()  : AUD_MN.GetMusicMasterVolume();
        const float sfxVolume    = sfxVolumeSlider    ? sfxVolumeSlider->GetValue()    : AUD_MN.GetSfxMasterVolume();
        const float deadzone     = deadzoneSlider     ? deadzoneSlider->GetValue()     : INP_MN.GetGamepadDeadzone();

        AUD_MN.SetMasterVolume(masterVolume);
        AUD_MN.SetMusicMasterVolume(musicVolume);
        AUD_MN.SetSfxMasterVolume(sfxVolume);
        INP_MN.SetGamepadDeadzone(deadzone);

        ApplyWindowSettings(settingsFullscreen, settingsVsync);
        INP_MN.SaveBindingsToJson(true);

        SaveRuntimeSettings({
            masterVolume,
            musicVolume,
            sfxVolume,
            settingsFullscreen,
            settingsVsync
        });

        statusLabel->SetText("Configuracion aplicada y guardada.");
    }

    void MainMenuScene::init()
    {
        RegisterPuzzleInputActions();
        const RuntimeSettingsData runtimeSettings = LoadRuntimeSettings();
        AUD_MN.SetMasterVolume(runtimeSettings.masterVolume);
        AUD_MN.SetMusicMasterVolume(runtimeSettings.musicVolume);
        AUD_MN.SetSfxMasterVolume(runtimeSettings.sfxVolume);
        settingsFullscreen = runtimeSettings.fullscreen;
        settingsVsync = runtimeSettings.vsync;
        ApplyWindowSettings(settingsFullscreen, settingsVsync);

        StartMenuMusic();
        // init animated menu background
        PuzzleRPG::MenuBackground::Get().Init();

        // add logo image as title (retain reference so we can hide/show it per panel)
        // Preserve a 1:1 aspect by using the smaller of the supplied width/height.
        // Compute X so the logo is centered above the main column of buttons
        float buttonsCenterX = kFirstButton.x + (kButtonSize.x * 0.5f);
        glm::vec2 logoRawSize(600.0f, 300.0f);
        float logoSide = std::min(logoRawSize.x, logoRawSize.y);
        glm::vec2 logoPos(buttonsCenterX - (logoSide * 0.5f), 80.0f);
        logoImage = new CosmicEngine::UIImage("ui/logo", logoPos, glm::vec2(logoSide, logoSide), true);
        UI_MN.AddElement(logoImage);

        // restore retained text title (some logic references `title`)
        title = new CosmicEngine::UIText("", "puzzle_font_big",
                 glm::vec2(660.0f, 60.0f),
                 glm::vec2(600.0f, 120.0f), true);
        title->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
        UI_MN.AddElement(title);

        statusLabel = new CosmicEngine::UIText("", "puzzle_font",
                                               glm::vec2(520.0f, 940.0f),
                                               glm::vec2(900.0f, 50.0f), true);
        statusLabel->SetTextColor(glm::vec3(0.9f, 0.9f, 0.5f));
        UI_MN.AddElement(statusLabel);

        backButton = CreateMenuButton("Volver", glm::vec2(80.0f, 940.0f), glm::vec2(220.0f, 70.0f),
            [this]()
            {
                SetPanel(MenuPanel::Root);
            });

        BuildRootButtons();
        BuildLevelButtons();

        nameField = new CosmicEngine::UITextField(GameSession::Get().username, "Nombre",
                                                  "puzzle_font",
                                                  glm::vec2(680.0f, 280.0f),
                                                  glm::vec2(560.0f, 70.0f),
                                                  24, false);
        UI_MN.AddElement(nameField);

        ipField = new CosmicEngine::UITextField(GameSession::Get().hostIp, "IP",
                                                "puzzle_font",
                                                glm::vec2(680.0f, 380.0f),
                                                glm::vec2(380.0f, 70.0f),
                                                40, false);
        UI_MN.AddElement(ipField);

        portField = new CosmicEngine::UITextField(std::to_string(GameSession::Get().port), "Puerto",
                                                  "puzzle_font",
                                                  glm::vec2(1080.0f, 380.0f),
                                                  glm::vec2(160.0f, 70.0f),
                                                  5, false);
        UI_MN.AddElement(portField);

        hostButton = CreateMenuButton("Crear partida (Host)", glm::vec2(680.0f, 500.0f), glm::vec2(560.0f, 80.0f),
            [this]()
            {
                auto &session = GameSession::Get();
                session.username = nameField ? nameField->GetText() : session.username;
                try { session.port = std::stoi(portField->GetText()); } catch (...) { session.port = 25566; }
                SetPanel(MenuPanel::HostLevelSelect);
            });
        hostButton->SetVisible(false);

        joinButton = CreateMenuButton("Unirse por IP", glm::vec2(680.0f, 600.0f), glm::vec2(560.0f, 80.0f),
            [this]()
            {
                auto &session = GameSession::Get();
                session.mode     = GameSession::NetworkMode::Client;
                session.username = nameField ? nameField->GetText() : session.username;
                session.hostIp   = ipField ? ipField->GetText() : session.hostIp;
                try { session.port = std::stoi(portField->GetText()); } catch (...) { session.port = 25566; }
                if (NET_MN.ConnectToServer(session.hostIp, session.port, session.username))
                {
                    SCN_MN.ReplaceScene(new GameplayScene());
                }
                else
                {
                    statusLabel->SetText("No se pudo conectar a " + session.hostIp + ":" + std::to_string(session.port));
                }
            });
        joinButton->SetVisible(false);

        multiplayerElements = { nameField, ipField, portField, hostButton, joinButton };

        newLevelName = new CosmicEngine::UITextField("nuevo_nivel", "Nombre del nivel",
                                                     "puzzle_font",
                                                     glm::vec2(1080.0f, 240.0f),
                                                     glm::vec2(420.0f, 70.0f),
                                                     32, false);
        UI_MN.AddElement(newLevelName);

        createLevelButton = CreateMenuButton("Crear nivel nuevo", glm::vec2(1080.0f, 330.0f), glm::vec2(420.0f, 70.0f),
            [this]()
            {
                std::string name = newLevelName ? newLevelName->GetText() : "nuevo_nivel";
                if (name.empty()) name = "nuevo_nivel";
                SCN_MN.ReplaceScene(new EditorScene(name, true));
            });
        createLevelButton->SetVisible(false);

        editorCreateElements = { newLevelName, createLevelButton };

        BuildSettingsPanel();
        BuildCreditsPanel();

        creditsButton = CreateMenuButton("Creditos", glm::vec2(1590.0f, 930.0f), glm::vec2(260.0f, 60.0f),
            [this]() { SetPanel(MenuPanel::Credits); });

        versionLabel = new CosmicEngine::UIText("v1.0.0", "puzzle_font",
                                                glm::vec2(1590.0f, 998.0f),
                                                glm::vec2(260.0f, 28.0f), true);
        versionLabel->SetTextColor(glm::vec3(0.6f, 0.6f, 0.6f));
        UI_MN.AddElement(versionLabel);

        SetPanel(MenuPanel::Root);
    }

    void MainMenuScene::update(double deltaTime)
    {
        // animated background update
        PuzzleRPG::MenuBackground::Get().Update(deltaTime);

        if (captureIndex < 0) return;

        if (captureArmFrames > 0)
        {
            --captureArmFrames;
            return;
        }

        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyEventType::KeyDown))
        {
            CancelCapture();
            if (statusLabel) statusLabel->SetText("Captura cancelada.");
            return;
        }

        const std::size_t idx = static_cast<std::size_t>(captureIndex);
        if (captureForGamepad)
        {
            const int button = INP_MN.GetFirstGamepadButtonJustPressed();
            if (button >= 0)
            {
                ApplyCapturedGamepad(idx, button);
                CancelCapture();
            }
        }
        else
        {
            const int key = INP_MN.GetFirstKeyJustPressed();
            if (key > 0 && key != GLFW_KEY_ESCAPE)
            {
                ApplyCapturedKeyboard(idx, key);
                CancelCapture();
            }
        }
    }
    void MainMenuScene::BuildCreditsPanel()
    {
        const glm::vec3 categoryColor(0.95f, 0.85f, 0.30f);
        const glm::vec3 nameColor(0.92f, 0.92f, 0.92f);
        // All entries share the same x/width so UIText centers them at the same screen x (960).
        const glm::vec2 entryPos(560.0f, 0.0f);
        const glm::vec2 entrySize(800.0f, 55.0f);

        auto addCat = [&](const std::string &text, float y)
        {
            auto *label = new CosmicEngine::UIText(text, "puzzle_font_big",
                                                   glm::vec2(entryPos.x, y), entrySize, false);
            label->SetTextColor(categoryColor);
            UI_MN.AddElement(label);
            creditsElements.push_back(label);
        };
        auto addName = [&](const std::string &text, float y)
        {
            auto *label = new CosmicEngine::UIText(text, "puzzle_font_big",
                                                   glm::vec2(entryPos.x, y), entrySize, false);
            label->SetTextColor(nameColor);
            UI_MN.AddElement(label);
            creditsElements.push_back(label);
        };

        addCat("Programación",                                   210.0f);
        addName("Francisco Rosa",                                262.0f);

        addCat("Arte / Sprites",                                 337.0f);
        addName("Irma Abarca",                                   389.0f);
        addName("CraftPix.net",                                  441.0f);
        addName("Francisco Rosa",                                493.0f);

        addCat("Musica",                                         568.0f);
        addName("alkakrab",                                      620.0f);

        addCat("Tipografia",                                     695.0f);
        addName("Monogram Extended  -  datagoblin",              747.0f);

        addCat("Diseño de niveles",                              822.0f);
        addName("Francisco Rosa",                                874.0f);
    }

    void MainMenuScene::draw()
    {
        // draw animated background behind UI
        PuzzleRPG::MenuBackground::Get().Draw();
    }
    void MainMenuScene::reset() {}
}
