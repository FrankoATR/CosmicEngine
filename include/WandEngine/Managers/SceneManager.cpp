#include "SceneManager.hpp"

#include "../Models/GameScene.hpp"

namespace WandEngine
{

    SceneManager::SceneManager()
    {
        this->BackBufferColor = WAND_COLOR(1.0f, 2.0f, 3.0f, 0.0f);
        this->isRunning = true;
        this->NextScene = nullptr;
    }


    void SceneManager::PushScene(GameScene *scene)
    {
        sceneStack.push_back(scene);
        sceneStack.back()->LoadResources();
        sceneStack.back()->StartLoadingThread();
    }

    void SceneManager::ReplaceScene(GameScene *scene)
    {
        NextScene = scene;
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
            else
            {
                sceneStack.back()->LoadResources();
                sceneStack.back()->StartLoadingThread();
            }
        }
    }

    void SceneManager::Update(double deltaTime)
    {
        if (NextScene != nullptr)
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

            sceneStack.push_back(NextScene);
            sceneStack.back()->LoadResources();
            sceneStack.back()->StartLoadingThread();

            NextScene = nullptr;
        }

        if (!sceneStack.empty())
        {
            GameScene* currentScene = sceneStack.back();

            bool progressComplete = currentScene->IsProgressLoadingSceneComplete();
            bool threadActive = currentScene->IsLoadingThreadJoinable();
            
            if (progressComplete)
            {
                if (threadActive)
                {
                    currentScene->JoinLoadingThread();
                }

                if (!threadActive) 
                {
                    currentScene->UpdateManagers(deltaTime);
                }
                else
                {
                    currentScene->UpdateLoadingScene(deltaTime);
                }
            }
            else
            {
                currentScene->UpdateLoadingScene(deltaTime);
            }
        }
        else
        {
            isRunning = false;
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
                currentScene->Draw();
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

        if (NextScene != nullptr)
        {
            if (NextScene->IsLoadingThreadJoinable())
            {
                NextScene->JoinLoadingThread();
            }
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
