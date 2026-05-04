#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../Managers/GameManager.h"
#include "GameBodyObject.h"

enum Object {
    DynamicEntity,
    StaticEntity
};

class GameObject
{
private:

    std::string ObjectName;
    Object ObjectType;
    int ObjectId;
    ALLEGRO_BITMAP* Sprite;
    Vec2 Position;
    Vec2 LastPosition;
    Vec2 Size;
    short int LayerId;

    bool AliveInGameManager;

public:
    GameManager* Game;

    GameObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    virtual void Draw();
    virtual void Update(float deltaTime);
    virtual void OnCollision(GameObject* other);

    void SetPosition(Vec2 NewPosition);
    Vec2 GetPosition();

    void SetSize(Vec2 NewSize);
    Vec2 GetSize();

    std::string GetObjectName();
    Object GetObjectType();

    void SetObjectId(int NewObjectId);
    int GetObjectId();


    void SetLayerId(short int NewLayerId);
    short int GetLayerId();
    
    void SetSprite(ALLEGRO_BITMAP* NewSprite);
    ALLEGRO_BITMAP* GetSprite();

    void SetToLastPosition();
    void Destroy();
    bool GetAliveInGameManager();
    
    virtual ~GameObject();
};



#endif