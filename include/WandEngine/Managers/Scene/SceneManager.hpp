#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace WandEngine
{

    class GameScene;

    class SceneManager
    {
    private:
        std::vector<GameScene *> sceneStack;
        bool isRunning;
        glm::vec3 BackgroundColor;
        GameScene* NextScene;
        
        SceneManager();
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

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
        void Draw();

        bool Empty() const;
        void Clear();

        bool IsSceneLoaded() const;
        bool Running() const;

        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor();

    };

}

#endif // SCENEMANAGER_HPP
