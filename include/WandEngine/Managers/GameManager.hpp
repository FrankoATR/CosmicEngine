#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#define GLFW_INCLUDE_NONE 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace WandEngine
{

    class GameTimer;
    class GameScene;

    class GameManager
    {

    private:
        bool fullScreenMode, VSyncEnable;
        float gameticks;
        double targetFPS;
        GameTimer* FPSTimer;

        GLFWwindow* window;

        GameManager();
        ~GameManager();

        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    public:


        static GameManager &GetInstance()
        {
            static GameManager instance;
            return instance;
        }

        void SetTargetFPS(double targetFPS);
        int GetCurrentFPS();

        void SetGameTicks(float gameTicks);
        float GetCurrenGameTicks();

        void EnableVsync();
        void DisableVsync();
        bool IsVSyncEnable();

        void SetFirstScene(GameScene *scene);

        void SetWindows_Size(glm::vec2 screenSize);
        void SetWindows_FullScreenMode();
        void SetWindows_WindowsMode(int width, int height);

        GLFWwindow* GetWindow();

        glm::vec2 GetWindowsSize();

        bool IsFullScreen();

        void EndProgram();

        bool Init(int screenWidth, int screenHeight);
        void Update();
        void Clear();
    };

}

#endif // GAMEMANAGER_HPP