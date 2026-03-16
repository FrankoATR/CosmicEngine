#ifndef COSMIC_GAMEMANAGER_HPP
#define COSMIC_GAMEMANAGER_HPP

#include "../interfaces/imanager_singleton.hpp"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

namespace CosmicEngine
{

    struct GameManagerInitParams : public ManagerInitParams
    {
        int screenWidth;
        int screenHeight;
        int baseScreenWidth;
        int baseScreenHeight;

        GameManagerInitParams(int sw, int sh, int bsw, int bsh)
            : screenWidth(sw), screenHeight(sh), baseScreenWidth(bsw), baseScreenHeight(bsh) {}
    };

    class Scene;

    class GameManager : public SingletonManager<GameManager>
    {
        friend class SingletonManager<GameManager>;

    private:
        GameManager() = default;
        ~GameManager() = default;

        GLFWwindow *window;
        glm::vec2 baseAspectSize;
        bool fullScreenMode, vsyncEnabled;

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

    public:
        void init(const ManagerInitParams &params) override;
        void update();
        void shutdown();

        void endprogram();
        void initManagers() const;

        void setTargetFps(double targetFPS);
        int getCurrentFps() const;

        void setGameTicks(float gameTicks);
        float getCurrenGameTicks() const;

        void enableVsync();
        void disableVsync();
        bool isVsyncEnabled();

        void toggleFullscreen();

        void setFirstScene(Scene *scene);

        void setWindowSize(glm::vec2 screenSize);
        void setWindowFullScreenMode();
        void setWindowWindowMode(int width, int height);
        GLFWwindow *getWindowPtr() const;
        glm::vec2 getWindowSize() const;

        void setFrameBufferSizeCallback(std::function<void(int width, int height)>);
        void setDropCallback(std::function<void(int count, const char **paths)>);
        void setMousePositionCallback(std::function<void(double xpos, double ypos)>);
        void setMouseScrollCallback(std::function<void(double xoffset, double yoffset)>);

        std::vector<std::string> getDroppedFiles();
        void pushDroppedFile(std::string filepath);

        void setBaseAspectSize(int baseAspectWidth, int baseAspectHeight);
        glm::vec2 getBaseAspectSize() const;
        bool isFullScreen() const;
    };

}

#endif // COSMIC_GAMEMANAGER_HPP