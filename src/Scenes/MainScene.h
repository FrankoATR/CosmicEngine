#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "../WandAllegroEngine/Models/GameScene.h"
#include "../Utilities/Paths.h"
#include "../Entities/LinkObject.h"


class MainScene : public GameScene
{
private:
    LinkObject* player;

public:
    MainScene(GameManager* Game);
    
    void Init() override;
    void Update(double deltaTime) override;

    void UpdateLoadingScene() override;

    ~MainScene();
};


#endif