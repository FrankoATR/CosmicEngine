#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <vector>

class GameScene;


namespace WandEngine
{

    class SceneManager
    {
    private:
        std::vector<GameScene *> sceneStack;
        bool sceneChanged = false;
        GameScene *nextScene;
        bool isRunning = true;

        SceneManager() = default;
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        void ChangeScene();

    public:
        static SceneManager &GetInstance()
        {
            static SceneManager instance;
            return instance;
        }

        void PushScene(GameScene *scene);
        void ReplaceScene(GameScene *scene);
        void PopScene();
        void Update(double deltaTime);

        void Clear();

        bool IsSceneLoaded();
        bool Running() const;
    };

}

#endif // SCENEMANAGER_HPP