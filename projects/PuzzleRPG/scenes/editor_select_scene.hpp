#ifndef PUZZLERPG_EDITOR_SELECT_SCENE_HPP
#define PUZZLERPG_EDITOR_SELECT_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_field.hpp>

#include <vector>

namespace PuzzleRPG
{
    // Pantalla previa al editor. Permite abrir un nivel existente o crear uno
    // nuevo a partir de una plantilla vacia.
    class EditorSelectScene : public CosmicEngine::Scene
    {
    public:
        EditorSelectScene();
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        CosmicEngine::UIText      *title;
        CosmicEngine::UIText      *statusLabel;
        CosmicEngine::UITextField *newLevelName;
        CosmicEngine::UIButton    *createButton;
        CosmicEngine::UIButton    *backButton;
        std::vector<CosmicEngine::UIButton *> levelButtons;
    };
}

#endif
