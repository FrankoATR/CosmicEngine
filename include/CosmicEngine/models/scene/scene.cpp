#include "scene.hpp"
#include "../../managers/resource/resource_manager.hpp"
#include "../../managers/body/body_manager.hpp"
#include "../../managers/object/object_manager.hpp"
#include "../../managers/game_manager.hpp"
#include "../../managers/event/event_manager.hpp"
#include "../../managers/audio/audio_manager.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"
#include "../../managers/timer/timer_manager.hpp"
#include "../../managers/light/light_manager.hpp"
#include "../../managers/ui/ui_manager.hpp"

#include <iostream>

namespace CosmicEngine
{
    Scene::Scene(std::string Name) : ProgressLoadingScene(0.0f), Name(Name)
    {
        this->ShowBodys = false;
        this->ShowGrid = false;
        this->ShowCamera = false;
    }

    void Scene::reset()
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
                                    { this->init(); });
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
        int W_width = CosmicEngine::GameManager::GetInstance().GetWindowsSize().x;
        //al_draw_filled_rectangle(50, W_width / 2 - 10, 50 + 800 * GetProgressLoadingScene(), W_width / 2 + 10, al_map_rgb(0, 255, 0));
    }
*/

    void Scene::draw()
    {

    }

    void Scene::UpdateManagers(double deltaTime)
    {
        update(deltaTime);


        ObjectManager::GetInstance().update(deltaTime);
        BodyManager::GetInstance().update();
        LightManager::GetInstance().update(deltaTime);

        /*
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        */
        UIManager::GetInstance().update(deltaTime);
        /*
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        */

    }

    void Scene::DrawManagers()
    {
        this->draw();
        
        ObjectManager::GetInstance().draw();

        if (ShowBodys)
        {
            BodyManager::GetInstance().draw();
        }

        if (ShowGrid)
        {
            //Adaptarlo
        }

        if (ShowCamera)
        {
            //CameraManager::GetInstance().draw();
        }

    #if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        glDisable(GL_DEPTH_TEST);
    #endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        UIManager::GetInstance().draw();
        InputManager::GetInstance().DrawMouseSprite();

        glDisable(GL_BLEND);
    #if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        glEnable(GL_DEPTH_TEST);
    #endif
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
        AudioManager::GetInstance().Clear();
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
