#ifndef COSMIC_SCENEMANAGER_HPP
#define COSMIC_SCENEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
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
        
        void init();
        void shutdown();
        void update(double deltaTime);
        void draw();

        void PushScene(Scene *scene);
        void ReplaceScene(Scene *scene);
        void PopScene();


        bool empty() const;
        void Clear();

        bool IsSceneLoaded() const;
        bool Running() const;

        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor();

    };

}

#endif // COSMIC_SCENEMANAGER_HPP
