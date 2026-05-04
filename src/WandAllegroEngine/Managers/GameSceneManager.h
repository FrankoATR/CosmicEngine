#ifndef GAMESCENEMANAGER_H
#define GAMESCENEMANAGER_H

#include <vector>

class GameScene;

class GameSceneManager {
private:
    std::vector<GameScene*> sceneStack;
    bool sceneChanged = false;
    GameScene* nextScene;
    bool isRunning = true;
    void ChangeScene();

public:
    GameSceneManager();
    ~GameSceneManager();
    void PushScene(GameScene* scene);
    void ReplaceScene(GameScene* scene);
    void PopScene();
    void Update(double deltaTime);

    void Clear();

    bool IsSceneLoaded();
    bool Running() const;
};


#endif