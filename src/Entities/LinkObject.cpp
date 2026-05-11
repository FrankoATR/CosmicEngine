#include "LinkObject.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/TimerManager.hpp"

#include <math.h>

LinkObject::LinkObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font) : GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    this->DestructorMode = true;
    this->IsTPActive = false;
    this->Velocity = 400;
}

void LinkObject::Init()
{
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
    //ChangeSpriteTimer = new GameTimer(0.5, true, false);
    //TimerManager::GetInstance().Add(ChangeSpriteTimer);
}

void LinkObject::Draw()
{
    GameObject::Draw();

    if (font)
    {
        al_draw_text(font, al_map_rgba(255, 0, 100, 0), GetPosition().x, GetPosition().y - 60, 0, GetObjectName().c_str());
        al_draw_text(font, al_map_rgba(255, 255, 255, 0), GetPosition().x - 100, GetPosition().y + 60, 0, ("X: " + std::to_string((int)GetPosition().x) + ", " + "Y: " + std::to_string((int)GetPosition().y)).c_str());
    }
}

void LinkObject::Update(float deltaTime)
{

    /*
    
    if(!GetInsideGridArea())
    {
        SetToLastPosition();
    }
    
    GameObject::Update(deltaTime);

    */

    bool KeyARelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, KeyRelease);
    bool KeyDRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, KeyRelease);
    bool KeyWRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, KeyRelease);
    bool KeySRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, KeyRelease);
    

    if (KeyARelease && KeyWRelease)
    {
        MoveUpLeft(deltaTime);
    }
    else if (KeyWRelease && KeyDRelease)
    {
        MoveUpRight(deltaTime);
    }
    else if (KeyDRelease && KeySRelease)
    {
        MoveDownRight(deltaTime);
    }
    else if (KeySRelease && KeyARelease)
    {
        MoveDownLeft(deltaTime);
    }
    else if (KeyARelease)
    {
        MoveLeft(deltaTime);
    }
    else if (KeyDRelease)
    {
        MoveRight(deltaTime);
    }
    else if (KeyWRelease)
    {
        MoveUp(deltaTime);
    }
    else if (KeySRelease)
    {
        MoveDown(deltaTime);
    }



    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_E, KeyDown))
    {
        DestructorMode = !DestructorMode;
    }


    if(0)
    {
        IsTPActive = false;
    }

}

void LinkObject::MoveUp(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y - Velocity * deltaTime)));
}

void LinkObject::MoveDown(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y + Velocity * deltaTime)));
}

void LinkObject::MoveRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + Velocity * deltaTime, (GetPosition().y)));
}

void LinkObject::MoveLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - Velocity * deltaTime, (GetPosition().y)));
}


    
void LinkObject::MoveUpLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y - (Velocity / sqrt(2.0f)) * deltaTime));

}

void LinkObject::MoveUpRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y - (Velocity / sqrt(2.0f)) * deltaTime));
}

void LinkObject::MoveDownLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y + (Velocity / sqrt(2.0f)) * deltaTime));

}

void LinkObject::MoveDownRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y + (Velocity / sqrt(2.0f)) * deltaTime));
}



void LinkObject::OnCollision(GameObject *other)
{

    if (other->GetObjectName() == "Emerson")
    {

        if (DestructorMode)
        {
            SetToLastPosition();
        }
        else
        {
            other->Destroy();
        }

        // EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");

        if (!IsTPActive)
        {
            // EventManager::GetInstance().TriggerEvent<GameObject *, GameObject *>("ChangePosition", this, other);
        }
    }
    if (other->GetObjectName() == "Tile")
    {
        if (DestructorMode)
        {
            SetToLastPosition();
        }
        else
        {
            other->Destroy();
        }
    }

    if (other->GetObjectType() == Object::DynamicEntity)
    {
    }
}
