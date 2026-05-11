#include "LinkObject.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"

LinkObject::LinkObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font) : GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    this->DestructorMode = true;
    this->IsTPActive = false;
    this->TimeToTP = 3;
    this->last_time = 0;

}

void LinkObject::Init()
{
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
}

void LinkObject::Draw()
{
    GameObject::Draw();

    if (font)
    {
        al_draw_text(font, al_map_rgba(255, 0, 100, 0), GetPosition().x, GetPosition().y - 60, 0, GetObjectName().c_str());
    }
}

void LinkObject::Update(float deltaTime)
{

    GameObject::Update(deltaTime);

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, KeyRelease))
    {
        MoveLeft(deltaTime);
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, KeyRelease))
    {
        MoveRight(deltaTime);
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, KeyRelease))
    {
        MoveUp(deltaTime);
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, KeyRelease))
    {
        MoveDown(deltaTime);
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_Q, KeyDown))
    {
        SetSize(WAND_VEC2(64, 64));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_E, KeyDown))
    {
        DestructorMode = !DestructorMode;
    }

    double current_time = al_get_time();
    if (current_time - last_time >= 1.0)
    {
        last_time = current_time;


        if(IsTPActive){
            TimeToTP--;
            std::cout << "SEG" << std:: endl;
            if (TimeToTP <= 0) {
                IsTPActive = false;
                TimeToTP = 3;
            }
        }

    }

}

void LinkObject::MoveUp(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y - 400 * deltaTime)));
}

void LinkObject::MoveDown(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y + 400 * deltaTime)));
}

void LinkObject::MoveRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + 400 * deltaTime, (GetPosition().y)));
}

void LinkObject::MoveLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - 400 * deltaTime, (GetPosition().y)));
}

void LinkObject::OnCollision(GameObject *other)
{

    if (GetObjectName() == "Player")
    {
        if (other->GetObjectName() == "Emerson")
        {

            // EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");

            if (!IsTPActive)
            {
                EventManager::GetInstance().TriggerEvent<GameObject *, GameObject *>("ChangePosition", this, other);
            }
        }
        if (other->GetObjectName() == "Tile")
        {
            DestructorMode ? other->Destroy() : SetToLastPosition();
            EventManager::GetInstance().TriggerEvent("OnTileDestroy");
        }
    }
    else{
        if (other->GetObjectName() == "Player")
        {
            SetToLastPosition();
            //other->Destroy();
            //EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");
        }
        if (other->GetObjectName() == "Tile")
        {
            SetToLastPosition();
        }

    }




    if (other->GetObjectType() == Object::DynamicEntity)
    {
    }
}
