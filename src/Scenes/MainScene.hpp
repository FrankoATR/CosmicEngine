#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <WandEngine/Models/GameScene.hpp>
#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Player;

class MainScene : public GameScene
{
private:
    int CurrentLevel;

    Player* player;
    int focusObjID;
    float ostVolume;
    int fpsSliderValue;
    int ticksSliderValue;
    bool vsynEnable;

    bool showPanel;

    glm::vec2 cameraVelocity;

    std::string currentMusic;

    glm::vec2 mouseInitialPosForArea;
    glm::vec2 mouseFinalPosForArea;
    bool mouseKeyDownForArea;

public:
    MainScene(int Level, int Attempts);
    
    void Init() override;
    void Draw() override;
    void Update(double deltaTime) override;
    void LoadResources() override;
    void LoadMap();

};


#endif // MAINSCENE_HPP