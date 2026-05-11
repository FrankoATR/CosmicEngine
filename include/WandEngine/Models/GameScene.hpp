#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <string>
#include <mutex>
#include <queue>
#include <functional>
#include <thread>

namespace WandEngine
{
    class GameScene
    {
    private:
        float ProgressLoadingScene;
        std::string Name;
        //std::thread loadingThread;
        //std::mutex progressMutex;

        bool ShowBodys;
        bool ShowGrid;
        bool ShowCamera;

    public:
        GameScene(std::string Name);
        virtual ~GameScene();

        virtual void Init() = 0;
        virtual void Reset();
        virtual void Draw();
        virtual void Update(double deltaTime) = 0;
        virtual void LoadResources() = 0;

        //virtual void UpdateLoadingScene(double deltaTime); // Utilizar en caso de animaciones en la pantalla de carga
        //virtual void DrawLoadingScene();

        void UpdateManagers(double deltaTime);
        void DrawManagers();

        //void SetProgressLoadingScene(float Progress);
        //float GetProgressLoadingScene();
        bool IsProgressLoadingSceneComplete();

        std::string GetName();
        void Clear();

        //void AddMainThreadTask(std::function<void()> task);
        //void ExecuteMainThreadTasks();

        //void StartLoadingThread();
        //bool IsLoadingThreadJoinable() const;
        //void JoinLoadingThread();

        void ToogleShowBodys();
        void ToogleShowGrid();
        void ToogleShowCamera();
    };
}

#endif // GAMESCENE_HPP
