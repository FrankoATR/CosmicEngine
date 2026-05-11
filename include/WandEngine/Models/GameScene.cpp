#include "GameScene.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../Managers/BodyManager.hpp"
#include "../Managers/ObjectManager.hpp"
#include "../Managers/GameManager.hpp"
#include "../Managers/EventManager.hpp"
#include "../Managers/MusicManager.hpp"
#include "../Managers/SoundManager.hpp"
#include "../Managers/CameraManager.hpp"
#include "../Managers/InputManager.hpp"
#include "../Managers/TimerManager.hpp"
#include "../Managers/UIManager.hpp"

#include <iostream>

namespace WandEngine
{
    GameScene::GameScene(std::string Name) : ProgressLoadingScene(0.0f), Name(Name)
    {
        this->ShowBodys = false;
        this->ShowGrid = false;
        this->ShowCamera = false;
    }


    void GameScene::SetProgressLoadingScene(float Progress)
    {
        std::lock_guard<std::mutex> lock(progressMutex);
        if (Progress > 1.0f)
            ProgressLoadingScene = 1.0f;
        else if (Progress < 0.0f)
            ProgressLoadingScene = 0.0f;
        else
            ProgressLoadingScene = Progress;
    }

    float GameScene::GetProgressLoadingScene()
    {
        std::lock_guard<std::mutex> lock(progressMutex);
        return ProgressLoadingScene;
    }

    bool GameScene::IsProgressLoadingSceneComplete()
    {
        return GetProgressLoadingScene() >= 1.0f;
    }

    std::string GameScene::GetName()
    {
        return Name;
    }


    void GameScene::StartLoadingThread()
    {
        loadingThread = std::thread([this]()
                                    { this->Init(); });
    }

    bool GameScene::IsLoadingThreadJoinable() const
    {
        return loadingThread.joinable();
    }

    void GameScene::JoinLoadingThread()
    {
        if (loadingThread.joinable())
        {
            loadingThread.join();
        }
    }

    void GameScene::UpdateLoadingScene(double deltaTime)
    {
        WAND_SIZE pos = GameManager::GetInstance().GetWindowsSize();
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(pos.width/2, pos.height/2));
    }

    void GameScene::DrawLoadingScene()
    {
        int W_width = WandEngine::GameManager::GetInstance().GetWindowsSize().width;
        al_draw_filled_rectangle(50, W_width / 2 - 10, 50 + 800 * GetProgressLoadingScene(), W_width / 2 + 10, al_map_rgb(0, 255, 0));
    }

    void GameScene::UpdateManagers(double deltaTime)
    {
        Update(deltaTime);
        UIManager::GetInstance().Update(deltaTime);
        BodyManager::GetInstance().Update();
        ObjectManager::GetInstance().Update(deltaTime);

    }

    void GameScene::Draw()
    {
        ObjectManager::GetInstance().Draw();

        if (ShowBodys)
        {
            BodyManager::GetInstance().Draw();
        }

        if (ShowGrid)
        {
            //Adaptarlo
        }

        if (ShowCamera)
        {
            CameraManager::GetInstance().Draw();
        }

        UIManager::GetInstance().Draw();
        InputManager::GetInstance().DrawMouseSprite();
    }

    void GameScene::ToogleShowBodys()
    {
        this->ShowBodys = !this->ShowBodys;
    }

    void GameScene::ToogleShowGrid()
    {
        this->ShowGrid = !this->ShowGrid;
    }

    void GameScene::ToogleShowCamera()
    {
        this->ShowCamera = !this->ShowCamera;
    }


    void GameScene::Clear()
    {
        MusicManager::GetInstance().Clear();
        SoundManager::GetInstance().Clear();
        BodyManager::GetInstance().Clear();
        ObjectManager::GetInstance().Clear();
        ResourceManager::GetInstance().Clear();
        EventManager::GetInstance().Clear();
        TimerManager::GetInstance().Clear();
        UIManager::GetInstance().Clear();
        InputManager::GetInstance().ResetMouseSettings();

        if (loadingThread.joinable())
        {
            loadingThread.join();
        }

        #ifndef NDEBUG
            std::cout << "SCENE **" << GetName() << "** CLEARED" << std::endl;
		#endif
    }


    GameScene::~GameScene()
    {
        #ifndef NDEBUG
            std::cout << "SCENE **" << GetName() << "** DESTROYED" << std::endl;
		#endif
    }

}
