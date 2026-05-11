#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#define GLFW_INCLUDE_NONE 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

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

        std::vector<std::string> droppedfiles;

        GameManager();
        ~GameManager();

        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void DropCallback(GLFWwindow *window, int count, const char **paths); // Deberia hacer un metodo para cada setcallback?

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

        std::vector<std::string> GetDroppedFiles();
        void PushDroppedFile(std::string filepath);


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