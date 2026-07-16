#ifndef PUZZLERPG_MAIN_MENU_SCENE_HPP
#define PUZZLERPG_MAIN_MENU_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_slider.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/derived/ui_image.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_field.hpp>
#include <CosmicEngine/models/ui/ui_element.hpp>

#include <functional>
#include <string>
#include <vector>

namespace PuzzleRPG
{
    // Menu principal. Botones:
    //   - Jugar (single-player local, abre selector de nivel).
    //   - Editor (abre la pantalla de seleccion de niveles del editor).
    //   - Multijugador (host / unirse).
    //   - Salir.
    class MainMenuScene : public CosmicEngine::Scene
    {
    public:
        MainMenuScene();
        ~MainMenuScene() override = default;
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        enum class MenuPanel
        {
            Root,
            SinglePlayerLevels,
            Multiplayer,
            HostLevelSelect,
            EditorLevels,
            Settings,
            Credits
        };

        CosmicEngine::UIText  *title;
        CosmicEngine::UIImage *logoImage;
        CosmicEngine::UIText  *statusLabel;
        CosmicEngine::UIButton *backButton;

        CosmicEngine::UITextField *nameField;
        CosmicEngine::UITextField *ipField;
        CosmicEngine::UITextField *portField;
        CosmicEngine::UITextField *newLevelName;
        CosmicEngine::UIButton    *hostButton;
        CosmicEngine::UIButton    *joinButton;
        CosmicEngine::UIButton    *createLevelButton;
        CosmicEngine::UISlider    *masterVolumeSlider;
        CosmicEngine::UISlider    *musicVolumeSlider;
        CosmicEngine::UISlider    *sfxVolumeSlider;
        CosmicEngine::UISlider    *deadzoneSlider;
        CosmicEngine::UIButton    *fullscreenButton;
        CosmicEngine::UIButton    *vsyncButton;
        CosmicEngine::UIButton    *applySettingsButton;
        CosmicEngine::UIText      *captureHintLabel;

        std::vector<CosmicEngine::UIButton *> rootButtons;
        std::vector<CosmicEngine::UIButton *> singleLevelButtons;
        std::vector<CosmicEngine::UIButton *> hostLevelButtons;
        std::vector<CosmicEngine::UIButton *> editorLevelButtons;
        std::vector<CosmicEngine::UIElement *> multiplayerElements;
        std::vector<CosmicEngine::UIElement *> editorCreateElements;
        std::vector<CosmicEngine::UIElement *> settingsElements;
        std::vector<CosmicEngine::UIText *> settingsInputLabels;
        std::vector<CosmicEngine::UIText *> settingsKeyboardLabels;
        std::vector<CosmicEngine::UIText *> settingsGamepadLabels;
        std::vector<CosmicEngine::UIButton *> settingsKeyboardCaptureButtons;
        std::vector<CosmicEngine::UIButton *> settingsGamepadCaptureButtons;
        std::vector<CosmicEngine::UIElement *> creditsElements;
        CosmicEngine::UIButton *creditsButton;
        CosmicEngine::UIText   *versionLabel;

        MenuPanel currentPanel;
        bool resourcesLoaded;
        bool settingsFullscreen;
        bool settingsVsync;
        int  captureIndex;
        bool captureForGamepad;
        int  captureArmFrames;

        void SetPanel(MenuPanel panel);
        void SetGroupVisible(const std::vector<CosmicEngine::UIElement *> &elements, bool visible);
        void BuildRootButtons();
        void BuildLevelButtons();
        void BuildSettingsPanel();
        void BuildCreditsPanel();
        void RefreshSettingsFields();
        void UpdateSettingsToggleLabels();
        void UpdateBindingDisplay(std::size_t index);
        void StartCapture(std::size_t index, bool gamepad);
        void CancelCapture();
        void ApplyCapturedKeyboard(std::size_t index, int keyCode);
        void ApplyCapturedGamepad(std::size_t index, int buttonCode);
        void ApplySettingsFromFields();
        CosmicEngine::UIButton *CreateMenuButton(const std::string &label,
                                                 glm::vec2 position,
                                                 glm::vec2 size,
                                                 std::function<void()> onClick);
    };
}

#endif
