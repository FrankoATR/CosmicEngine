#ifndef GAMEEDITORSCENE_HPP
#define GAMEEDITORSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"
#include "../WandAllegroEngine/Models/GameObject.hpp"

using namespace WandEngine;

class GameEditorScene : public GameScene
{
private:
    WAND_VEC2 CurrentMousePosition;
    WAND_VEC2 LastMousePosition;
    GameObject* ObjectInHand;
    double last_time;

public:
    GameEditorScene();
    
    void Init() override;
    void Update(double deltaTime) override;
};


#endif // GAMEEDITORSCENE_HPP