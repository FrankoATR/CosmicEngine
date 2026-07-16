#ifndef PUZZLERPG_MULTIPLAYER_LOBBY_SCENE_HPP
#define PUZZLERPG_MULTIPLAYER_LOBBY_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_field.hpp>

namespace PuzzleRPG
{
    // Lobby de multijugador. Permite anfitrionar una partida o unirse a un
    // servidor por IP. El puerto por defecto es 25566.
    class MultiplayerLobbyScene : public CosmicEngine::Scene
    {
    public:
        MultiplayerLobbyScene();
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        CosmicEngine::UIText       *title;
        CosmicEngine::UIText       *statusLabel;
        CosmicEngine::UITextField  *ipField;
        CosmicEngine::UITextField  *portField;
        CosmicEngine::UITextField  *nameField;
        CosmicEngine::UIButton     *hostButton;
        CosmicEngine::UIButton     *joinButton;
        CosmicEngine::UIButton     *backButton;
    };
}

#endif
