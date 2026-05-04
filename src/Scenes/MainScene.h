#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "../WandAllegroEngine/Models/GameScene.h"

class MainScene : public GameScene
{
private:

public:
    MainScene(GameManager* Game);
    
    void Init() override;
    void Update(double deltaTime) override;

    void UpdateLoadingScene() override;

    ~MainScene();
};


#endif