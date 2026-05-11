#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "GameBodyObject.hpp"

namespace WandEngine
{

    class GameObject
    {
    private:
        std::string ObjectName;
        Object ObjectType;
        int ObjectId;
        ALLEGRO_BITMAP *Sprite;
        WAND_VEC2 Position;
        WAND_VEC2 LastPosition;
        WAND_VEC2 Size;
        short int LayerId;

        bool Visible;
        bool InsideGridArea;
        bool AliveInGameManager;

    public:
        GameObject() = delete;
        GameObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId);
        virtual void Draw();
        virtual void Init();
        virtual void Update(float deltaTime);
        virtual void OnCollision(GameObject *other);

        void SetPosition(WAND_VEC2 NewPosition);
        WAND_VEC2 GetPosition() const;

        void SetSize(WAND_VEC2 NewSize);
        WAND_VEC2 GetSize() const;

        std::string GetObjectName() const;
        void SetObjectName(std::string NewName);

        Object GetObjectType() const;

        void SetObjectId(int NewObjectId);
        int GetObjectId() const;

        void SetLayerId(short int NewLayerId);
        short int GetLayerId() const;

        void SetSprite(ALLEGRO_BITMAP *NewSprite);
        ALLEGRO_BITMAP *GetSprite() const;

        void SetToLastPosition();

        void SetVisible(bool Visible);
        bool GetVisible();

        void SetInsideGridArea(bool InsideGridArea);
        bool GetInsideGridArea();

        void Destroy();
        bool GetAliveInGameManager() const;

        virtual ~GameObject();
    };

}

#endif // GAMEOBJECT_HPP