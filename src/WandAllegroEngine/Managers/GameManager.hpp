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

struct Size {
    int Width;
    int Height;
    Size(int Width, int Height) : Width(Width), Height(Height){}
};

class GameScene;

class GameManager {

private:
    bool redraw;
    bool ShowBodys;
    ALLEGRO_COLOR BackBufferColor;

public:
    Size ScreenSize;

    ALLEGRO_DISPLAY* Window;
    ALLEGRO_EVENT_QUEUE* event_queue;

    ALLEGRO_TIMER* FPS;
    ALLEGRO_EVENT Event;

    std::map<int, bool> keyState;


    double lastTime, currentTime, deltaTime;

    GameManager(Size ScreenSize);
    ~GameManager();

    void ToggleShowBody();

    void SetBackBufferColor(ALLEGRO_COLOR color);
    void SetFirstScene(GameScene* scene);

    void SetWindows_Size(Size ScreenSize);
    void SetWindows_FullScreenMode();
    void SetWindows_WindowsMode();
    Size GetWindowsSize();

    bool IsFullScreen();
    
    bool Init();
    void Update();
    void End();


};


#endif // GAMEMANAGER_HPP