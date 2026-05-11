#include "SceneManager.hpp"
#include "ObjectManager.hpp"
#include "BodyManager.hpp"
#include "CameraManager.hpp"

#include "../Models/GameScene.hpp"

namespace WandEngine
{

    SceneManager::SceneManager()
    {
        this->BackBufferColor = WAND_COLOR(1.0f, 2.0f, 3.0f, 0.0f);
        this->isRunning = true;
    }

    void SceneManager::StartSceneLoading(GameScene *scene)
    {
        scene->StartLoadingThread();
    }

    void SceneManager::PushScene(GameScene *scene)
    {
        sceneStack.push_back(scene);
        StartSceneLoading(scene);
    }

    void SceneManager::ReplaceScene(GameScene *scene)
    {
        if (!sceneStack.empty())
        {
            if (sceneStack.back()->IsLoadingThreadJoinable())
            {
                sceneStack.back()->JoinLoadingThread();
            }
            sceneStack.back()->Clear();
            delete sceneStack.back();
            sceneStack.pop_back();
        }

        sceneStack.push_back(scene);
        StartSceneLoading(scene);
    }

    void SceneManager::PopScene()
    {
        if (!sceneStack.empty())
        {
            if (sceneStack.back()->IsLoadingThreadJoinable())
            {
                sceneStack.back()->JoinLoadingThread();
            }
            sceneStack.back()->Clear();
            delete sceneStack.back();
            sceneStack.pop_back();

            if (sceneStack.empty())
            {
                isRunning = false;
            }
        }
    }

    void SceneManager::Update(double deltaTime)
    {
        if (!sceneStack.empty())
        {
            GameScene *currentScene = sceneStack.back();

            if (currentScene->IsProgressLoadingSceneComplete())
            {
                if (currentScene->IsLoadingThreadJoinable())
                {
                    currentScene->JoinLoadingThread();
                }

                currentScene->ExecuteMainThreadTasks();
                currentScene->Update(deltaTime);
                ObjectManager::GetInstance().Update(deltaTime);
                BodyManager::GetInstance().Update();
            }
            else
            {
                currentScene->ExecuteMainThreadTasks();
                currentScene->UpdateLoadingScene();
            }
        }
    }

    void SceneManager::Draw()
    {
        if (!sceneStack.empty())
        {
            GameScene *currentScene = sceneStack.back();

            al_clear_to_color(al_map_rgba(BackBufferColor.r, BackBufferColor.g, BackBufferColor.b, BackBufferColor.a));

            if (currentScene->IsProgressLoadingSceneComplete())
            {
                ObjectManager::GetInstance().Draw();
                //BodyManager::GetInstance().Draw();
                CameraManager::GetInstance().Draw();
            }
            else
            {
                currentScene->DrawLoadingScene();
            }

            al_flip_display();
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

    void SceneManager::SetBackBufferColor(WAND_COLOR color)
    {
        this->BackBufferColor = color;
    }

    void SceneManager::Clear()
    {
        while (!sceneStack.empty())
        {
            if (sceneStack.back()->IsLoadingThreadJoinable())
            {
                sceneStack.back()->JoinLoadingThread();
            }
            sceneStack.back()->Clear();
            delete sceneStack.back();
            sceneStack.pop_back();
        }
        isRunning = false;
        std::cout << "Scene manager cleared" << std::endl;
    }

    SceneManager::~SceneManager()
    {
        Clear();
        std::cout << "Scene manager destroyed" << std::endl;
    }
}
