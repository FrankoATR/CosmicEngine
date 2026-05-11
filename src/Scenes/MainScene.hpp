#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <WandEngine/Models/Scene/Scene.hpp>
#include <WandEngine/Models/Object/Object.hpp>

using namespace WandEngine;

class Player;

class MainScene : public Scene
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

    bool EditorMode;
    Object* selectedObject = nullptr;
    float lineMusicPos = 0.0f;

    std::string droppedlevelfilepath = "";
    std::string droppedmusicfilepath = "";
    std::string fileName = "";


public:
    MainScene();
    
    void Init() override;
    void Reset() override;
    void Draw() override;
    void Update(double deltaTime) override;
    void LoadResources() override;

};


#endif // MAINSCENE_HPP