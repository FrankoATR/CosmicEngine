#ifndef COSMIC_SCENE_HPP
#define COSMIC_SCENE_HPP

#include <string>

namespace CosmicEngine
{
    class Scene
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
        Scene(std::string Name);
        virtual ~Scene();

        virtual void init() = 0;
        virtual void reset();
        virtual void draw();
        virtual void update(double deltaTime) = 0;
        virtual void loadResources() = 0;

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

#endif // COSMIC_SCENE_HPP
