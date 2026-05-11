#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"
#include "../WandAllegroEngine/Models/UIElements/UIButton.hpp"
#include "../WandAllegroEngine/Models/UIElements/UIText.hpp"
#include "../WandAllegroEngine/Models/GameTimer.hpp"

using namespace WandEngine;

class MainScene : public GameScene
{
private:
    bool InitialSound;
    WAND_VEC2 CurrentMousePosition;
    WAND_VEC2 LastMousePosition;
    bool DeathSound;
    GameTimer* WaitToReInitTimer;
    UIText* Text1;
    UIText* Label_GameOver;
    UIText* Label_EnemiesDestroyed;
    std::vector<UIText* > textsEntities;

    int EnemiesDestroyed;

public:
    MainScene();
    
    void Init() override;
    void Update(double deltaTime) override;
};


#endif // MAINSCENE_HPP