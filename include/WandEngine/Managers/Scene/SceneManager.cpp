#include "SceneManager.hpp"

#include "../../Models/GameScene.hpp"
#include <iostream>

namespace WandEngine
{

    SceneManager::SceneManager()
    {
        this->BackgroundColor = glm::vec3(0.10f, 0.12f, 0.13f);
        this->isRunning = true;
        this->NextScene = nullptr;
    }

    void SceneManager::PushScene(GameScene *scene)
    {
        sceneStack.push_back(scene);
        sceneStack.back()->Init();
    }

    void SceneManager::ReplaceScene(GameScene *scene)
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

    void SceneManager::Update(double deltaTime)
    {
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

    void SceneManager::Draw()
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

    bool SceneManager::Empty() const
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

        #ifndef NDEBUG
                std::cout << "Scene manager cleared" << std::endl;
        #endif
    }

    SceneManager::~SceneManager()
    {
        #ifndef NDEBUG
                std::cout << "Scene manager destroyed" << std::endl;
        #endif
    }
}
