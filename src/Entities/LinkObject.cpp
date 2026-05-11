#include "LinkObject.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/TimerManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SoundManager.hpp"
#include "ProjectileObject.hpp"

#include <math.h>

LinkObject::LinkObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font) : 
    GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    this->DestructorMode = true;
    this->IsTPActive = false;
    this->Velocity = 400;
}

void LinkObject::Init()
{
    BodyManager::GetInstance().Add(this, Position, GetSize());
    WaitShootTimer = new GameTimer(0.05, true, true);
    TimerManager::GetInstance().Add(WaitShootTimer);
}

void LinkObject::Draw()
{
    GameObject::Draw();

    if (font)
    {
        al_draw_text(font, al_map_rgba(255, 0, 100, 0), Position.x, Position.y - 60, 0, GetObjectName().c_str());
        al_draw_text(font, al_map_rgba(255, 255, 255, 0), Position.x - 100, Position.y + 60, 0, ("X: " + std::to_string((int)Position.x) + ", " + "Y: " + std::to_string((int)Position.y)).c_str());
    }
}

void LinkObject::Update(float deltaTime)
{

    LastPosition = Position;

    bool KeyARelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, KeyEventType::KeyRelease);
    bool KeyDRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, KeyEventType::KeyRelease);
    bool KeyWRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, KeyEventType::KeyRelease);
    bool KeySRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, KeyEventType::KeyRelease);
    

    if (KeyARelease && KeyWRelease)  //Puedo usar -1 a 1,   0.5 si quiero tener movimiento constante
    {
        Direction.x = -1;
        Direction.y = -1;
    }
    else if (KeyWRelease && KeyDRelease)
    {
        Direction.x = 1;
        Direction.y = -1;
    }
    else if (KeyDRelease && KeySRelease)
    {
        Direction.x = 1;
        Direction.y = 1;
    }
    else if (KeySRelease && KeyARelease)
    {
        Direction.x = -1;
        Direction.y = 1;
    }
    else if (KeyARelease)
    {
        Direction.x = -1;
        Direction.y = 0;
    }
    else if (KeyDRelease)
    {
        Direction.x = 1;
        Direction.y = 0;
    }
    else if (KeyWRelease)
    {
        Direction.x = 0;
        Direction.y = -1;
    }
    else if (KeySRelease)
    {
        Direction.x = 0;
        Direction.y = 1;
    }


    UpdatePosition(Velocity, deltaTime);

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_E, KeyEventType::KeyDown))
    {
        DestructorMode = !DestructorMode;
    }

    if (DestructorMode && InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyRelease))
    {
        MoveForDirection(InputManager::GetInstance().GetMousePosition(), 0.05, deltaTime);
    }
    else
    {
        Direction.x = 0;
        Direction.y = 0;
    }
    
    if (!DestructorMode && InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyRelease))
    {
        if(WaitShootTimer->IsTrigger())
        {
            ProjectileObject* projectile = new ProjectileObject(Object::DynamicEntity, Position, WAND_VEC2(16, 16), "Projectile", Sprite, 3, this, InputManager::GetInstance().GetMousePosition());
            ObjectManager::GetInstance().Add(projectile);
            SoundManager::GetInstance().Play("Shoot", 1.0f, false);
        }
        WaitShootTimer->Play();
    }
    else if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyUp))
    {
        WaitShootTimer->Pause();
        WaitShootTimer->Reset();
    }
    


    if(0)
    {
        IsTPActive = false;
    }

}


void LinkObject::OnCollision(GameObject *other)
{

    if (other->GetObjectName() == "Enemy")
    {

        if (DestructorMode)
        {
            SetToLastPosition();
        }
        else
        {
            other->Destroy();
        }

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


LinkObject::~LinkObject()
{
    WaitShootTimer->End();
}