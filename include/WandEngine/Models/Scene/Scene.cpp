#include "Scene.hpp"
#include "../../Managers/Resource/ResourceManager.hpp"
#include "../../Managers/Body/BodyManager.hpp"
#include "../../Managers/Object/ObjectManager.hpp"
#include "../../Managers/GameManager.hpp"
#include "../../Managers/Event/EventManager.hpp"
#include "../../Managers/Audio/Music/MusicManager.hpp"
#include "../../Managers/Audio/Sound/SoundManager.hpp"
#include "../../Managers/Camera/CameraManager.hpp"
#include "../../Managers/Input/InputManager.hpp"
#include "../../Managers/Timer/TimerManager.hpp"
#include "../../Managers/UI/UIManager.hpp"

#include <iostream>

namespace WandEngine
{
    Scene::Scene(std::string Name) : ProgressLoadingScene(0.0f), Name(Name)
    {
        this->ShowBodys = false;
        this->ShowGrid = false;
        this->ShowCamera = false;
    }

    void Scene::Reset()
    {

    }

    bool Scene::IsProgressLoadingSceneComplete()
    {
        //return GetProgressLoadingScene() >= 1.0f;
        return true;
    }

    std::string Scene::GetName()
    {
        return Name;
    }


/*
    void Scene::SetProgressLoadingScene(float Progress)
    {
        std::lock_guard<std::mutex> lock(progressMutex);
        if (Progress > 1.0f)
            ProgressLoadingScene = 1.0f;
        else if (Progress < 0.0f)
            ProgressLoadingScene = 0.0f;
        else
            ProgressLoadingScene = Progress;
    }

    float Scene::GetProgressLoadingScene()
    {
        std::lock_guard<std::mutex> lock(progressMutex);
        return ProgressLoadingScene;
    }

    void Scene::StartLoadingThread()
    {
        loadingThread = std::thread([this]()
                                    { this->Init(); });
    }

    bool Scene::IsLoadingThreadJoinable() const
    {
        return loadingThread.joinable();
    }

    void Scene::JoinLoadingThread()
    {
        if (loadingThread.joinable())
        {
            loadingThread.join();
        }
    }

    void Scene::UpdateLoadingScene(double deltaTime)
    {
        glm::vec2 pos = GameManager::GetInstance().GetWindowsSize();
        CameraManager::GetInstance().FocusPosition(glm::vec2(pos.x/2, pos.y/2));
    }

    void Scene::DrawLoadingScene()
    {
        int W_width = WandEngine::GameManager::GetInstance().GetWindowsSize().x;
        //al_draw_filled_rectangle(50, W_width / 2 - 10, 50 + 800 * GetProgressLoadingScene(), W_width / 2 + 10, al_map_rgb(0, 255, 0));
    }
*/

    void Scene::Draw()
    {

    }

    void Scene::UpdateManagers(double deltaTime)
    {
        Update(deltaTime);
        ObjectManager::GetInstance().Update(deltaTime);
        BodyManager::GetInstance().Update();
        UIManager::GetInstance().Update(deltaTime);

    }

    void Scene::DrawManagers()
    {
        this->Draw();
        
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
            //CameraManager::GetInstance().Draw();
        }

        UIManager::GetInstance().Draw();
        InputManager::GetInstance().DrawMouseSprite();
    }

    void Scene::ToogleShowBodys()
    {
        this->ShowBodys = !this->ShowBodys;
    }

    void Scene::ToogleShowGrid()
    {
        this->ShowGrid = !this->ShowGrid;
    }

    void Scene::ToogleShowCamera()
    {
        this->ShowCamera = !this->ShowCamera;
    }


    void Scene::Clear()
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

        /*
        if (loadingThread.joinable())
        {
            loadingThread.join();
        }
        */

        #ifndef NDEBUG
            std::cout << "SCENE **" << GetName() << "** CLEARED" << std::endl;
		#endif
    }


    Scene::~Scene()
    {
        #ifndef NDEBUG
            std::cout << "SCENE **" << GetName() << "** DESTROYED" << std::endl;
		#endif
    }

}
