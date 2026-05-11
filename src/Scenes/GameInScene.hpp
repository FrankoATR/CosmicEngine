#ifndef GAMEINSCENE_HPP
#define GAMEINSCENE_HPP

#include "../WandAllegroEngine/Models/GameScene.hpp"
#include "../Utilities/Paths.hpp"

class GameInScene : public GameScene
{
private:

public:
    GameInScene();
    
    void Init() override;
    void Update(double deltaTime) override;

    void UpdateLoadingScene() override;

    ~GameInScene();
};


#endif // GAMEINSCENE_HPP