#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <WandEngine/Models/GameScene.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include "../Entities/Player.hpp"

using namespace WandEngine;

class MainScene : public GameScene
{
private:
    GameTimer* Respawn_Timer;

public:
    MainScene();
    
    void Init() override;
    void Update(double deltaTime) override;
};


#endif // MAINSCENE_HPP