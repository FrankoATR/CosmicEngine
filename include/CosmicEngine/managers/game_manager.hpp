#ifndef COSMIC_GAMEMANAGER_HPP
#define COSMIC_GAMEMANAGER_HPP

/**
 * @file game_manager.hpp
 * @brief Declares the main application manager responsible for bootstrapping and running the engine.
 */

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

namespace CosmicEngine
{
    /**
     * @brief Groups the initial desktop window size and the virtual base resolution used by the engine.
     */
    struct GameManagerInitParams
    {
        /** @brief Initial GLFW window title. */
        std::string windowTitle;
        /** @brief Optional image path for the GLFW window icon. */
        std::string windowIconPath;
        /** @brief Initial window width in pixels. */
        int screenWidth;
        /** @brief Initial window height in pixels. */
        int screenHeight;
        /** @brief Virtual base width used by camera and UI calculations. */
        int baseScreenWidth;
        /** @brief Virtual base height used by camera and UI calculations. */
        int baseScreenHeight;
        /** @brief Whether the render viewport should stretch to the window instead of preserving the base aspect ratio. */
        bool scaleViewportToWindow;
        /** @brief Whether the native window can be resized by the user. */
        bool windowResizable;
        /** @brief Whether windowed resizing should keep the base render aspect ratio. */
        bool lockWindowAspectToBaseRender;
        /** @brief Whether the application should start in fullscreen mode. */
        bool startFullscreen;
        /** @brief Whether the application should start with vsync enabled. */
        bool startVsync;

        /**
         * @brief Builds a startup parameter block.
         * @param title Initial window title.
         * @param iconPath Optional window icon path.
         * @param sw Initial window width in pixels.
         * @param sh Initial window height in pixels.
         * @param bsw Base virtual width.
         * @param bsh Base virtual height.
         * @param scaleViewport Whether the render viewport should stretch to the window instead of preserving the base aspect ratio.
         * @param resizable Whether the native window can be resized by the user.
         * @param lockAspect Whether windowed resizing should keep the base render aspect ratio.
         * @param fullscreen Whether the application should start in fullscreen mode.
         * @param vsync Whether the application should start with vsync enabled.
         */
        GameManagerInitParams(const std::string &title, const std::string &iconPath, int sw, int sh, int bsw, int bsh, bool scaleViewport = false, bool resizable = true, bool lockAspect = false, bool fullscreen = false, bool vsync = false)
            : windowTitle(title), windowIconPath(iconPath), screenWidth(sw), screenHeight(sh), baseScreenWidth(bsw), baseScreenHeight(bsh), scaleViewportToWindow(scaleViewport), windowResizable(resizable), lockWindowAspectToBaseRender(lockAspect), startFullscreen(fullscreen), startVsync(vsync) {}
    };

    class Scene;

    /**
     * @brief Coordinates the engine lifetime, main loop, window, and global manager initialization.
     *
     * GameManager is the central singleton that bootstraps the GLFW window, initializes
     * every engine subsystem (input, camera, resources, physics, etc.), and runs the
     * main game loop.  The typical lifecycle is:
     *
     * 1. Create a GameManagerInitParams block with the desired window and virtual sizes.
     * 2. Call init() to create the window and set up all managers.
     * 3. Call setFirstScene() with the initial scene.
     * 4. Call update() which enters the main loop and does not return until the game ends.
     * 5. Call shutdown() to release all resources.
     *
     * @par Example — minimal main.cpp
     * @code
     * #include <CosmicEngine/managers/game_manager.hpp>
     * #include <CosmicEngine/utils/logger.hpp>
     * #include "scenes/main_scene.hpp"
     *
     * int main()
     * {
     *     CosmicEngine::Logger::init(true, true);
     *
    *     CosmicEngine::GameManagerInitParams params{"My Game", "", 1280, 720, 1920, 1080, false, true};
     *     CosmicEngine::GameManager::GetInstance().init(params);
     *     CosmicEngine::GameManager::GetInstance().setFirstScene(new MainScene);
     *     CosmicEngine::GameManager::GetInstance().update();
     *     CosmicEngine::GameManager::GetInstance().shutdown();
     *
     *     CosmicEngine::Logger::shutdown();
     *     return 0;
     * }
     * @endcode
     *
     * @par Example — runtime controls (inside a scene update)
     * @code
     * GM_MN.enableVsync();        // enable vertical synchronization
     * GM_MN.toggleFullscreen();   // switch between windowed / fullscreen
     * GM_MN.endprogram();         // request engine shutdown
     * @endcode
     */
    class GameManager
    {
    private:
        GameManager() = default;
        ~GameManager() = default;
        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;

        GLFWwindow *window;
        glm::vec2 baseAspectSize;
        bool fullScreenMode, vsyncEnabled;
        bool scaleViewportToWindow = false;
        bool windowResizable = true;
        bool lockWindowAspectToBaseRender = false;

        float gameticks;
        double targetFPS;
        double fpsTimer;
        int frameCount;
        int currentFPS;

        std::vector<std::string> droppedfiles;
        std::vector<std::string> gameStates; // "0-PLAYING", "1-PAUSE" ... CREATE GAMESTATEMANAGER? MODULARIZE FOR UI ENGINE WITH FOR LOOP INTO SCENE->UPDATE()

        std::function<void(int, int)> framebufferSizeCallback;
        std::function<void(int, const char **)> dropCallback;
        std::function<void(double, double)> mousePositionCallback;
        std::function<void(double, double)> mouseScrollCallback;
        bool initialized = false;
        bool imguiInitialized = false;

    public:
        /** @brief Returns the singleton instance of the game manager. */
        static GameManager &GetInstance();

        /**
         * @brief Initializes the engine runtime and desktop window.
         * @param params Startup parameters for the window and base virtual resolution.
         * @throws std::runtime_error When a critical subsystem cannot be initialized.
         */
        void init(const GameManagerInitParams &params);
        /** @brief Runs the main update and render loop until the application is terminated. */
        void update();
        /** @brief Shuts down the runtime and releases global resources with best-effort cleanup across managers. */
        void shutdown();

        /** @brief Requests the application to terminate. */
        void endprogram();
        /** @brief Initializes manager singletons after the window and graphics context are ready.
         *  Critical manager failures are treated as fatal startup errors so the engine never continues with a partial runtime.
         *  @throws std::runtime_error When a critical manager fails to initialize.
         */
        void initManagers() const;

        /**
         * @brief Sets the presentation frame-rate target.
         * @param targetFPS Desired target frames per second.
         */
        void setTargetFps(double targetFPS);
        /**
         * @brief Returns the last measured frame-rate value.
         * @return Current frames per second.
         */
        int getCurrentFps() const;

        /**
         * @brief Sets the logical game-tick multiplier.
         * @param gameTicks Tick multiplier used by the runtime.
         */
        void setGameTicks(float gameTicks);
        /**
         * @brief Returns the configured logical game-tick multiplier.
         * @return Current game-tick value.
         */
        float getCurrenGameTicks() const;

        /** @brief Enables vertical synchronization. */
        void enableVsync();
        /** @brief Disables vertical synchronization. */
        void disableVsync();
        /**
         * @brief Returns whether vertical synchronization is currently enabled.
         * @return True when VSync is active.
         */
        bool isVsyncEnabled();

        /** @brief Toggles between windowed and fullscreen modes. */
        void toggleFullscreen();

        /**
         * @brief Sets the first scene that will be managed by the runtime.
         * @param scene Scene instance that becomes the initial active scene.
         */
        void setFirstScene(Scene *scene);

        /**
         * @brief Resizes the desktop window.
         * @param screenSize New window size in pixels.
         */
        void setWindowSize(glm::vec2 screenSize);
        /** @brief Updates the GLFW window title at runtime. */
        void setWindowTitle(const std::string &title);
        /** @brief Loads and applies a GLFW window icon from an image file. */
        void setWindowIcon(const std::string &iconPath);
        /** @brief Applies the configured resize and aspect-lock policy to the current native window. */
        void applyWindowResizePolicy();
        /** @brief Switches the current window into fullscreen mode. */
        void setWindowFullScreenMode();
        /**
         * @brief Switches the current window into windowed mode.
         * @param width Window width in pixels.
         * @param height Window height in pixels.
         */
        void setWindowWindowMode(int width, int height);
        /**
         * @brief Returns the native GLFW window handle.
         * @return Pointer to the managed GLFW window.
         */
        GLFWwindow *getWindowPtr() const;
        /**
         * @brief Returns the current desktop window size.
         * @return Window size in pixels.
         */
        glm::vec2 getWindowSize() const;

        /**
         * @brief Installs the framebuffer resize callback used by the runtime.
         *
         * The callback receives the new width and height in pixels.
         */
        void setFrameBufferSizeCallback(std::function<void(int width, int height)>);
        /**
         * @brief Installs the drag-and-drop callback used by the runtime.
         *
         * The callback receives the number of dropped files and their paths.
         */
        void setDropCallback(std::function<void(int count, const char **paths)>);
        /**
         * @brief Installs the mouse-position callback used by the runtime.
         *
         * The callback receives the cursor x and y position.
         */
        void setMousePositionCallback(std::function<void(double xpos, double ypos)>);
        /**
         * @brief Installs the mouse-scroll callback used by the runtime.
         *
         * The callback receives the horizontal and vertical scroll offsets.
         */
        void setMouseScrollCallback(std::function<void(double xoffset, double yoffset)>);

        /**
         * @brief Returns the list of files dropped onto the window since the last retrieval.
         * @return Collection of dropped file paths.
         */
        std::vector<std::string> getDroppedFiles();
        /**
         * @brief Appends a dropped file path to the internal queue.
         * @param filepath Path of the dropped file.
         */
        void pushDroppedFile(std::string filepath);

        /**
         * @brief Sets the base virtual aspect size used by the engine.
         * @param baseAspectWidth Virtual width.
         * @param baseAspectHeight Virtual height.
         */
        void setBaseAspectSize(int baseAspectWidth, int baseAspectHeight);
        /**
         * @brief Returns the base virtual aspect size used by the engine.
         * @return Base virtual size.
         */
        glm::vec2 getBaseAspectSize() const;
        /** @brief Returns whether the render viewport is configured to stretch to the full window. */
        bool isViewportScaledToWindow() const;
        /**
         * @brief Returns whether the desktop window is currently in fullscreen mode.
         * @return True when fullscreen mode is active.
         */
        bool isFullScreen() const;
    };

}

#endif // COSMIC_GAMEMANAGER_HPP