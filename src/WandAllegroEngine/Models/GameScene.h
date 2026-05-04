#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>

#include "../Managers/GameManager.h"

class GameScene
{
private:
    float ProgressLoadingScene;

public:
    GameManager* Game;
    ALLEGRO_THREAD *loadingThread = nullptr;
    
    GameScene(GameManager* Game);

    virtual void Init() = 0;
    virtual void Update(double deltaTime) = 0;

    virtual void UpdateLoadingScene();

    void SetProgressLoadingScene(float Progress);
    float GetProgressLoadingScene();
    bool IsProgressLoadinSceneComplete();

    void Clean();

    ~GameScene();
};


#endif