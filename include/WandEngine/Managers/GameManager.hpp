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
        GameManager();
        ~GameManager();
        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;

        bool fullScreenMode, VSyncEnable;
        float gameticks;
        double targetFPS;
        GameTimer *FPSTimer;

        GLFWwindow *window;

        std::vector<std::string> droppedfiles;

        glm::vec2 baseAspectSize;

        double fpsTimer;
        int frameCount;
        int currentFPS;

        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        static void DropCallback(GLFWwindow *window, int count, const char **paths); // Deberia hacer un metodo para cada setcallback?

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

        void SetFirstScene(GameScene *scene);

        void SetWindows_Size(glm::vec2 screenSize);
        void SetWindows_FullScreenMode();
        void SetWindows_WindowsMode(int width, int height);

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