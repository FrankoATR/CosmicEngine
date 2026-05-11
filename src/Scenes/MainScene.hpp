#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <WandEngine/Models/GameScene.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Models/UIElements/UIText.hpp>
#include "../Entities/Player.hpp"

using namespace WandEngine;

class MainScene : public GameScene
{
private:
    GameTimer* Respawn_Timer;
    GameTimer* CameraMovement_Timer;
    int Current_Level;
    int Current_Attempts;
    std::string current_music;

    UIText* Attempts_Label;

public:
    MainScene(int Level, int Attempts);
    
    void Init() override;
    void Update(double deltaTime) override;
    void LoadResources() override;
    void UpdateLoadingScene(double deltaTime) override;
    void LoadMap();
};


#endif // MAINSCENE_HPP