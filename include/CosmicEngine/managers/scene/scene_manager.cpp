/**
 * @file scene_manager.cpp
 * @brief Implements the scene stack manager used by the engine runtime.
 */

#include "scene_manager.hpp"

#include "../../models/scene/scene.hpp"
#include <iostream>

namespace CosmicEngine
{
    
    SceneManager &SceneManager::GetInstance()
    {
        static SceneManager instance;
        return instance;
    }

    SceneManager::SceneManager()
    {
        std::cout << "Scene manager created" << std::endl;
    }

    SceneManager::~SceneManager()
    {
        shutdown();
        std::cout << "Scene manager destroyed" << std::endl;
    }

    void SceneManager::init()
    {
        this->BackgroundColor = glm::vec3(0.10f, 0.12f, 0.13f);
        this->isRunning = true;
        this->NextScene = nullptr;

        std::cout << "Scene manager initialized" << std::endl;
    }

    void SceneManager::shutdown()
    {
        Clear();
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

        std::cout << "Scene manager cleared" << std::endl;
    }


}
