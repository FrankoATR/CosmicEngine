#ifndef PUZZLERPG_LEVEL_SELECT_SCENE_HPP
#define PUZZLERPG_LEVEL_SELECT_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>

#include <vector>

namespace PuzzleRPG
{
    // Pantalla de seleccion de nivel para jugar. Lista todos los niveles
    // existentes en assets/levels y, al elegir uno, pasa a GameplayScene.
    // En modo host, antes de pasar al juego se arranca el servidor.
    class LevelSelectScene : public CosmicEngine::Scene
    {
    public:
        explicit LevelSelectScene(bool willHostNetwork);
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        bool willHostNetwork;
        CosmicEngine::UIText *title;
        CosmicEngine::UIText *statusLabel;
        CosmicEngine::UIButton *backButton;
        std::vector<CosmicEngine::UIButton *> levelButtons;
    };
}

#endif
