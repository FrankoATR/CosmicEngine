#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"

using namespace WandEngine;

class MainScene : public GameScene
{
private:
    bool InitialSound;
    WAND_VEC2 CurrentMousePosition;
    WAND_VEC2 LastMousePosition;
    double last_time;
    double timeAfterDeath;
    bool DeathSound;

public:
    MainScene();
    
    void Init() override;
    void Update(double deltaTime) override;
};


#endif // MAINSCENE_HPP