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
        SceneManager();
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        std::vector<GameScene *> sceneStack;
        bool isRunning;
        glm::vec3 BackgroundColor;
        GameScene* NextScene;
        
    public:
        static SceneManager &GetInstance();
        
        void Init();
        void Shutdown();
        void Update(double deltaTime);
        void Draw();

        void PushScene(GameScene *scene);
        void ReplaceScene(GameScene *scene);
        void PopScene();


        bool Empty() const;
        void Clear();

        bool IsSceneLoaded() const;
        bool Running() const;

        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor();

    };

}

#endif // SCENEMANAGER_HPP
