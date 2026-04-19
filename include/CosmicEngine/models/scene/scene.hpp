#ifndef COSMIC_SCENE_HPP
#define COSMIC_SCENE_HPP

/**
 * @file scene.hpp
 * @brief Declares the abstract scene type used by the engine scene manager.
 */

#include <string>

namespace CosmicEngine
{
    /**
     * @brief Base class for all engine scenes.
     *
     * A scene encapsulates a runtime context and orchestrates scene-specific logic
     * together with the engine managers used for objects, bodies, UI, and rendering.
     * Derive from this class and implement init(), update(), and loadResources().
     * The GameManager drives the lifecycle: loadResources() is called once, then
     * init() once, and update()/draw() every frame until the scene is replaced.
     *
     * @par Example — declaring and registering a scene
     * @code
     * class MainScene : public CosmicEngine::Scene
     * {
     * public:
     *     MainScene() : Scene("MainScene") {}
    *     void init() override        { CreateSceneContent(); }
    *     void reset() override       { RestoreSceneState(); }
    *     void draw() override        { DrawSceneContent(); }
    *     void update(double dt) override { UpdateSceneLogic(dt); }
    *     void loadResources() override { LoadSceneTextures(); }
     * };
     *
     * // In main.cpp:
     * GameManager::GetInstance().setFirstScene(new MainScene);
     * @endcode
     *
     * @par Example — debug toggles inside a running scene
     * @code
     * // Toggle debug body wireframes, spatial grid, or camera helpers
     * ToogleShowBodys();
     * ToogleShowGrid();
     * ToogleShowCamera();
     * @endcode
     */
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
        /**
         * @brief Creates a new scene.
         * @param Name Logical name assigned to the scene.
         */
        Scene(std::string Name);
        /** @brief Releases the scene instance. */
        virtual ~Scene();

        /** @brief Performs scene-specific initialization logic. */
        virtual void init() = 0;
        /** @brief Restores scene-specific runtime state. */
        virtual void reset();
        /** @brief Draws scene-specific content before or after manager-driven rendering. */
        virtual void draw();
        /**
         * @brief Updates scene-specific logic.
         * @param deltaTime Fixed update time step.
         */
        virtual void update(double deltaTime) = 0;
        /** @brief Loads all scene-specific resources. */
        virtual void loadResources() = 0;

        //virtual void UpdateLoadingScene(double deltaTime); // Utilizar en caso de animaciones en la pantalla de carga
        //virtual void DrawLoadingScene();

        /**
         * @brief Updates the scene and all manager-owned content bound to it.
         * @param deltaTime Fixed update time step.
         */
        void UpdateManagers(double deltaTime);
        /** @brief Draws manager-owned content associated with the scene. */
        void DrawManagers();

        //void SetProgressLoadingScene(float Progress);
        //float GetProgressLoadingScene();
        /**
         * @brief Returns whether the scene loading phase is complete.
         * @return True when the loading progress has reached completion.
         */
        bool IsProgressLoadingSceneComplete();

        /**
         * @brief Returns the logical name assigned to the scene.
         * @return Scene name.
         */
        std::string GetName();
        /** @brief Clears manager-owned runtime data associated with the scene. */
        void Clear();

        //void AddMainThreadTask(std::function<void()> task);
        //void ExecuteMainThreadTasks();

        //void StartLoadingThread();
        //bool IsLoadingThreadJoinable() const;
        //void JoinLoadingThread();

        /** @brief Toggles debug rendering of physics bodies. */
        void ToogleShowBodys();
        /** @brief Toggles debug rendering of the spatial grid. */
        void ToogleShowGrid();
        /** @brief Toggles debug rendering of camera helpers. */
        void ToogleShowCamera();
    };
}

#endif // COSMIC_SCENE_HPP
