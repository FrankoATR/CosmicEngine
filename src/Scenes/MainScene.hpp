#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"

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


#endif // MAINSCENE_HPP