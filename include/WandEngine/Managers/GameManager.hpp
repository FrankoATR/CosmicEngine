#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../Utils/Configurations.hpp"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

namespace WandEngine
{

    class Timer;
    class Scene;

    class GameManager
    {
    private:
        GameManager();
        ~GameManager();
        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;

        bool fullScreenMode, VSyncEnable;
        float gameticks;
        double targetFPS;
        Timer *FPSTimer;

        GLFWwindow *window;

        std::vector<std::string> droppedfiles;

        glm::vec2 baseAspectSize;

        double fpsTimer;
        int frameCount;
        int currentFPS;

        std::function<void(int, int)> framebufferSizeCallback;
        std::function<void(int, const char**)> dropCallback;

    public:
        static GameManager &GetInstance();

        bool Init(int screenWidth, int screenHeight, int baseScreenWidth, int baseScreenHeight);
        void Update();
        void Shutdown();

        void EndProgram();

        bool InitManagers() const;

        void SetTargetFPS(double targetFPS);
        int GetCurrentFPS() const;

        void SetGameTicks(float gameTicks);
        float GetCurrenGameTicks() const;

        void EnableVsync();
        void DisableVsync();
        bool IsVSyncEnable();

        void SetFirstScene(Scene *scene);

        void SetWindows_Size(glm::vec2 screenSize);
        void SetWindows_FullScreenMode();
        void SetWindows_WindowsMode(int width, int height);

        void SetFramebufferSize_Callback(std::function<void(int width, int height)>);
        void SetDrop_Callback(std::function<void(int count, const char **paths)>);

        std::vector<std::string> GetDroppedFiles();
        void PushDroppedFile(std::string filepath);

        GLFWwindow *GetWindow() const;

        glm::vec2 GetWindowsSize() const;

        void SetBaseAspectSize(int baseAspectWidth, int baseAspectHeight);
        glm::vec2 GetBaseAspectSize() const;

        bool IsFullScreen() const;
    };

}

#endif // GAMEMANAGER_HPP