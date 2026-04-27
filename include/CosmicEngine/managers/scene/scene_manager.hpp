#ifndef COSMIC_SCENEMANAGER_HPP
#define COSMIC_SCENEMANAGER_HPP

/**
 * @file scene_manager.hpp
 * @brief Declares the scene stack manager used by the engine runtime.
 */

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{

    class Scene;

    /**
     * @brief Owns the scene stack and orchestrates scene transitions.
     *
     * SceneManager maintains a stack of Scene instances and drives their
     * lifecycle (load, init, update, draw).  The GameManager interacts with
     * SceneManager internally; usually you only call setFirstScene() on
     * GameManager and let it handle the rest.
     *
    * @par Example - pushing/replacing scenes (advanced)
     * @code
     * // Push a sub-scene on top (pauses the current one):
     * SCN_MN.PushScene(new PauseMenuScene);
     *
     * // Replace the current scene entirely:
     * SCN_MN.ReplaceScene(new Level2Scene);
     *
     * // Pop back to the previous scene:
     * SCN_MN.PopScene();
     * @endcode
     */
    class SceneManager
    {
    private:
        SceneManager();
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        std::vector<Scene *> sceneStack;
        bool isRunning;
        glm::vec3 BackgroundColor;
        Scene* NextScene;
        
    public:
        /** @brief Returns the singleton instance of the scene manager. */
        static SceneManager &GetInstance();
        
        /** @brief Initializes the scene manager state. */
        void init();
        /** @brief Shuts the scene manager down and releases managed scenes. */
        void shutdown();
        /** @brief Updates the active scene stack. */
        void update(double deltaTime);
        /** @brief Draws the active scene stack. */
        void draw();

        /** @brief Pushes a new scene onto the scene stack. */
        void PushScene(Scene *scene);
        /** @brief Replaces the current scene with a new one. */
        void ReplaceScene(Scene *scene);
        /** @brief Pops the current scene from the scene stack. */
        void PopScene();


        /**
         * @brief Returns whether the scene stack is empty.
         * @return The whether the scene stack is empty.
         */
        bool empty() const;
        /** @brief Clears and deletes every managed scene. */
        void Clear();

        /**
         * @brief Returns whether the current scene is fully loaded.
         * @return The whether the current scene is fully loaded.
         */
        bool IsSceneLoaded() const;
        /**
         * @brief Returns whether the scene manager is currently running.
         * @return The whether the scene manager is currently running.
         */
        bool Running() const;

        /**
         * @brief Sets the background color used by the renderer.
         * @param color Value provided by the caller.
         */
        void SetBackgroundColor(glm::vec3 color);
        /** @brief Returns the background color used by the renderer. */
        glm::vec3 GetBackgroundColor();

    };

}

#endif // COSMIC_SCENEMANAGER_HPP
