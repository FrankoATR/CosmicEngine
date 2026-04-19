/**
 * @file scene_manager.cpp
 * @brief Implements the scene stack manager used by the engine runtime.
 */

#include "scene_manager.hpp"

#include "../../models/scene/scene.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{
    
    SceneManager &SceneManager::GetInstance()
    {
        static SceneManager instance;
        return instance;
    }

    SceneManager::SceneManager()
    {
        RUNTIME_LIFECYCLE("Scene manager", "created");
    }

    SceneManager::~SceneManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Scene manager", "destroyed");
    }

    void SceneManager::init()
    {
        this->BackgroundColor = glm::vec3(0.10f, 0.12f, 0.13f);
        this->isRunning = true;
        this->NextScene = nullptr;

        RUNTIME_LIFECYCLE("Scene manager", "initialized");
    }

    void SceneManager::shutdown()
    {
        Clear();
        RUNTIME_LIFECYCLE("Scene manager", "shutdown");
    }


    void SceneManager::PushScene(Scene *scene)
    {
        sceneStack.push_back(scene);
        sceneStack.back()->init();
    }

    void SceneManager::ReplaceScene(Scene *scene)
    {
        if (NextScene != nullptr)
        {
            NextScene->Clear();
            delete NextScene;
        }
        NextScene = scene;
    }

    void SceneManager::PopScene()
    {
        if (!sceneStack.empty())
        {
            sceneStack.back()->Clear();
            delete sceneStack.back();
            sceneStack.pop_back();

            isRunning = !sceneStack.empty();
        }
    }

    void SceneManager::update(double deltaTime)
    {
        // Deferred replacement avoids deleting the active scene mid-update.
        if (NextScene)
        {
            PopScene();
            PushScene(NextScene);
            NextScene = nullptr;
        }

        if (!sceneStack.empty())
        {
            sceneStack.back()->UpdateManagers(deltaTime);
        }
        else
        {
            isRunning = false;
        }
    }

    void SceneManager::draw()
    {
        if (!sceneStack.empty() && sceneStack.back()) {
            sceneStack.back()->DrawManagers();
        }
    }

    bool SceneManager::IsSceneLoaded() const
    {
        if (!sceneStack.empty())
        {
            return sceneStack.back()->IsProgressLoadingSceneComplete();
        }
        return false;
    }

    bool SceneManager::Running() const
    {
        return isRunning;
    }

    bool SceneManager::empty() const
    {
        return sceneStack.empty();
    }

    void SceneManager::SetBackgroundColor(glm::vec3 color)
    {
        this->BackgroundColor = color;
    }

    glm::vec3 SceneManager::GetBackgroundColor()
    {
        return this->BackgroundColor;
    }

    void SceneManager::Clear()
    {
        while (!sceneStack.empty())
        {
            sceneStack.back()->Clear();
            delete sceneStack.back();
            sceneStack.pop_back();
        }
        sceneStack.clear();

        if (NextScene)
        {
            NextScene->Clear();
            delete NextScene;
            NextScene = nullptr;
        }

        isRunning = false;

        RUNTIME_LIFECYCLE("Scene manager", "cleared");
    }


}
