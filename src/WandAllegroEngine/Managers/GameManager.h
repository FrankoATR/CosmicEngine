#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

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

#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "GameSceneManager.h"
#include "GameBodyManager.h"

struct Size {
    int Width;
    int Height;
    Size(int Width, int Height) : Width(Width), Height(Height){}
};

class GameManager {

private:
    bool redraw;
    bool ShowBodys;
    ALLEGRO_COLOR BackBufferColor;

public:
    Size ScreenSize;

    ALLEGRO_DISPLAY* Window;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ResourceManager* resourceManager;
    GameObjectManager* gameObjectManager;
    GameSceneManager* gameSceneManager;
    GameBodyManager* gameBodyManager;

    ALLEGRO_TIMER* FPS;
    ALLEGRO_EVENT Event;

    std::map<int, bool> keyState;


    double lastTime, currentTime, deltaTime;

    GameManager(Size ScreenSize);
    ~GameManager();

    void ToggleShowBody();

    void SetBackBufferColor(ALLEGRO_COLOR color);

    void SetWindows_Size(Size ScreenSize);
    void SetWindows_FullScreenMode();
    void SetWindows_WindowsMode();
    Size GetWindowsSize();

    bool IsFullScreen();
    
    bool Init();
    void Update();
    void End();


};




#endif