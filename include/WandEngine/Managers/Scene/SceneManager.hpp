#ifndef WAND_SCENEMANAGER_HPP
#define WAND_SCENEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace WandEngine
{

    class Scene;

    class SceneManager
    {
    private:
        SceneManager();
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        std::vector<Scene *> sceneStack;
        bool isRunning;
        glm::vec3 BackgroundColor;
        Scene* NextScene;
        
    public:
        static SceneManager &GetInstance();
        
        void Init();
        void Shutdown();
        void Update(double deltaTime);
        void Draw();

        void PushScene(Scene *scene);
        void ReplaceScene(Scene *scene);
        void PopScene();


        bool Empty() const;
        void Clear();

        bool IsSceneLoaded() const;
        bool Running() const;

        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor();

    };

}

#endif // WAND_SCENEMANAGER_HPP
