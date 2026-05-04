#ifndef GAMEINSCENE_H
#define GAMEINSCENE_H

#include "../WandAllegroEngine/Models/GameScene.h"
#include "../Utilities/Paths.h"
#include "../Entities/LinkObject.h"

class GameInScene : public GameScene
{
private:
    LinkObject* player;
public:
    GameInScene(GameManager* Game);
    
    void Init() override;
    void Update(double deltaTime) override;

    void UpdateLoadingScene() override;

    ~GameInScene();
};


#endif