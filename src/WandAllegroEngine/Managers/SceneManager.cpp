#include "SceneManager.hpp"
#include "../Models/GameScene.hpp"

static void *InitLoadingScene_ThreadFunc(ALLEGRO_THREAD *thread, void *arg)
{
    GameScene *scene = (GameScene *)arg;
    std::cout << "THREAD 1" << std::endl;
    scene->Init();
    std::cout << "THREAD 2" << std::endl;
    return nullptr;
}

void WandEngine::SceneManager::ChangeScene()
{
    if (sceneChanged && nextScene)
    {
        if (!sceneStack.empty())
        {
            sceneStack.back()->Clear();
            al_destroy_thread(sceneStack.back()->loadingThread);
            delete sceneStack.back();
            sceneStack.pop_back();
        }
        sceneStack.push_back(nextScene);
        // sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
        // al_start_thread(sceneStack.back()->loadingThread);
        sceneStack.back()->Init();
        sceneChanged = false;
    }
}

void WandEngine::SceneManager::PushScene(GameScene *scene)
{
    if (sceneStack.empty())
    {
        sceneStack.push_back(scene);
        // sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
        // al_start_thread(sceneStack.back()->loadingThread);
        sceneStack.back()->Init();
        sceneChanged = false;
    }
    else
    {
        nextScene = scene;
        sceneChanged = true;
    }
}

void WandEngine::SceneManager::ReplaceScene(GameScene *scene)
{
    if (sceneStack.empty())
    {
        sceneStack.back()->Clear();
        delete sceneStack.back();
        sceneStack.pop_back();
        sceneStack.push_back(scene);
        // sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
        // al_start_thread(sceneStack.back()->loadingThread);
        sceneStack.back()->Init();
        sceneChanged = false;
    }
    else
    {
        nextScene = scene;
        sceneChanged = true;
    }
}

void WandEngine::SceneManager::PopScene()
{
    if (!sceneStack.empty())
    {
        sceneStack.back()->Clear();
        delete sceneStack.back();
        sceneStack.pop_back();
        if (!sceneStack.empty())
        {
            // sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
            // al_start_thread(sceneStack.back()->loadingThread);
            sceneStack.back()->Init();
        }
    }
}

void WandEngine::SceneManager::Update(double deltaTime)
{
    if (sceneChanged)
    {
        ChangeScene();
    }
    if (!sceneStack.empty())
    {
        sceneStack.back()->Update(deltaTime);

        /*
        if(sceneStack.back()->IsProgressLoadinSceneComplete()){
            sceneStack.back()->Update(deltaTime);
        }
        else{
            sceneStack.back()->UpdateLoadingScene();
        }
        */
    }
}

bool WandEngine::SceneManager::IsSceneLoaded()
{
    if (!sceneStack.empty())
    {
        if (sceneStack.back()->IsProgressLoadinSceneComplete())
        {
            if (sceneStack.back()->loadingThread)
            {
                al_join_thread(sceneStack.back()->loadingThread, nullptr);
                al_destroy_thread(sceneStack.back()->loadingThread);
                sceneStack.back()->loadingThread = nullptr;
            }
            return true;
        }
        else
        {
            false;
        }
    }
    return false;
}

bool WandEngine::SceneManager::Running() const
{
    return isRunning && !sceneStack.empty();
}

bool WandEngine::SceneManager::Empty()
{
    return sceneStack.empty();
}

void WandEngine::SceneManager::Clear()
{
    for (const auto &scene : sceneStack)
    {
        scene->Clear();
        delete scene;
    }
    sceneStack.clear();
    std::cout << "Scene manager clear" << std::endl;
}

WandEngine::SceneManager::~SceneManager()
{
    Clear();
    std::cout << "Scene manager destroyed" << std::endl;
}