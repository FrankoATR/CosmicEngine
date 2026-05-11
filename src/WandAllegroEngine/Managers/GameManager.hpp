#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <windows.h>
#include <map>

struct Size
{
    int Width;
    int Height;
    Size(int Width, int Height) : Width(Width), Height(Height) {}
};

class GameScene;

namespace WandEngine
{
    class GameManager
    {

    private:
        bool redraw;
        double lastTime, currentTime, deltaTime;

        ALLEGRO_DISPLAY *Window;
        ALLEGRO_TIMER *FPS;

        Size ScreenSize;

        GameManager();
        ~GameManager();

        GameManager(const GameManager &) = delete;
        GameManager &operator=(const GameManager &) = delete;

    public:

        ALLEGRO_EVENT_QUEUE *event_queue;
        ALLEGRO_EVENT Event;

        static GameManager &GetInstance()
        {
            static GameManager instance;
            return instance;
        }

        void SetFirstScene(GameScene *scene);

        void SetWindows_Size(Size ScreenSize);
        void SetWindows_FullScreenMode();
        void SetWindows_WindowsMode();
        Size GetWindowsSize();

        bool IsFullScreen();

        bool Init();
        void Update();
        void Clear();
    };

}

#endif // GAMEMANAGER_HPP