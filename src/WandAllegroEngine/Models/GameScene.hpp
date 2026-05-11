#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>

#include "../Managers/GameManager.hpp"

class GameScene
{
private:
    float ProgressLoadingScene;
    std::string Name;

public:
    ALLEGRO_THREAD *loadingThread = nullptr;
    
    GameScene(std::string Name);

    virtual void Init() = 0;
    virtual void Update(double deltaTime) = 0;

    virtual void UpdateLoadingScene();

    void SetProgressLoadingScene(float Progress);
    float GetProgressLoadingScene();
    bool IsProgressLoadinSceneComplete();

    std::string GetName();

    void Clear();

    ~GameScene();
};


#endif // GAMESCENE_HPP