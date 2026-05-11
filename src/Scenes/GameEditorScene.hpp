#ifndef GAMEEDITORSCENE_HPP
#define GAMEEDITORSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"
#include "../WandAllegroEngine/Models/GameObject.hpp"
#include "../WandAllegroEngine/Models/UIElements/UIText.hpp"

using namespace WandEngine;

class GameEditorScene : public GameScene
{
private:
    enum class HandMode
    {
        Move,
        Delete,
        Drag
    } ActualMode;

    WAND_VEC2 CurrentMousePosition;
    WAND_VEC2 LastMousePosition;
    GameObject *ObjectInHand;
    double last_time;

    UIText *Label_1, *Label_2, *Label_3, *Label_4;

public:
    GameEditorScene();

    void Init() override;
    void Update(double deltaTime) override;
};

#endif // GAMEEDITORSCENE_HPP