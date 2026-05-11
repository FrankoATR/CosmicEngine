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
#include "GameTimer.hpp"

namespace WandEngine
{

    class GameObject
    {
    private:
        GameTimer* MovementTimer;
        float VelocityForDuration;
    protected:
        std::string ObjectName;
        Object ObjectType;
        int ObjectId;
        ALLEGRO_BITMAP *Sprite;
        WAND_VEC2 Position;
        WAND_VEC2 LastPosition;
        WAND_VEC2 Direction;
        WAND_VEC2 ViewDirection;
        WAND_VEC2 Size;
        short int LayerId;  //0-254  char
        short int Rotation; //0-359
        float Velocity;
        //Utilizar vectores para indicar la direccion de visualizacion WAND_VEC2
        bool Visible;
        bool InsideGridArea;
        bool AliveInGameManager;

    public:
        GameObject() = delete;
        GameObject(GameObject* Other);
        GameObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId);
        virtual void Draw();
        virtual void Init();
        virtual void Update(float deltaTime);

        void SetPosition(WAND_VEC2 NewPosition);
        WAND_VEC2 GetPosition() const;

        void UpdatePosition(float Velocity, double DeltaTime);

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
        WAND_VEC2 GetLastPosition() const;

        void SetVisible(bool Visible);
        bool GetVisible();

        void SetDirection(WAND_VEC2 NewDirection);
        WAND_VEC2 GetDirection() const;

        void SetRotation(short int NewRotation);
        short int GetRotation() const; 

        void SetVelocity(float NewVelocity);
        float GetVelocity() const;

        bool ReachPositionInTime(WAND_VEC2 NewPosition, double Duration, double DeltaTime);
        bool MoveForDirection(WAND_VEC2 NewDirection, double Duration, double DeltaTime);

        void SetInsideGridArea(bool InsideGridArea);
        bool GetInsideGridArea();

        void Destroy();
        bool GetAliveInGameManager() const;

        GameObject* Clone() const{
            return new GameObject(*this);
        }

        virtual ~GameObject();
    };

}

#endif // GAMEOBJECT_HPP