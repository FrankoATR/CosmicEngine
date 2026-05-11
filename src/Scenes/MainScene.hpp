#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"

class MainScene : public GameScene
{
private:

public:
    MainScene();
    
    void Init() override;
    void Update(double deltaTime) override;

    ~MainScene();
};


#endif // MAINSCENE_HPP