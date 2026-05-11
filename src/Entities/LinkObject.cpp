#include "LinkObject.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"

LinkObject::LinkObject(Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font) : GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    this->DestructorMode = true;
    this->IsTPActive = false;
    this->TimeToTP = 3;
    this->last_time = 0;

}

void LinkObject::Init()
{
    WandEngine::BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
}

void LinkObject::Draw()
{
    GameObject::Draw();

    al_draw_text(font, al_map_rgba(255, 0, 100, 0), GetPosition().x, GetPosition().y - 60, NULL, GetObjectName().c_str());
}

void LinkObject::Update(float deltaTime)
{

    GameObject::Update(deltaTime);

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, WandEngine::KeyRelease))
    {
        MoveLeft(deltaTime);
    }
    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, WandEngine::KeyRelease))
    {
        MoveRight(deltaTime);
    }
    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, WandEngine::KeyRelease))
    {
        MoveUp(deltaTime);
    }
    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, WandEngine::KeyRelease))
    {
        MoveDown(deltaTime);
    }

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_Q, WandEngine::KeyDown))
    {
        SetSize(Vec2(64, 64));
    }
    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_E, WandEngine::KeyDown))
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
    SetPosition(Vec2(GetPosition().x, (GetPosition().y - 400 * deltaTime)));
}

void LinkObject::MoveDown(float deltaTime)
{
    SetPosition(Vec2(GetPosition().x, (GetPosition().y + 400 * deltaTime)));
}

void LinkObject::MoveRight(float deltaTime)
{
    SetPosition(Vec2(GetPosition().x + 400 * deltaTime, (GetPosition().y)));
}

void LinkObject::MoveLeft(float deltaTime)
{
    SetPosition(Vec2(GetPosition().x - 400 * deltaTime, (GetPosition().y)));
}

void LinkObject::OnCollision(GameObject *other)
{

    if (GetObjectName() == "Player")
    {
        if (other->GetObjectName() == "Emerson")
        {

            // WandEngine::EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");

            if (!IsTPActive)
            {
                WandEngine::EventManager::GetInstance().TriggerEvent<GameObject *, GameObject *>("ChangePosition", this, other);
            }
        }
    }
    else{
        if (other->GetObjectName() == "Player")
        {
            other->Destroy();
            WandEngine::EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");
        }
    }


    if (other->GetObjectName() == "Tile")
    {
        DestructorMode ? other->Destroy() : SetToLastPosition();
        WandEngine::EventManager::GetInstance().TriggerEvent("OnTileDestroy");
    }

    if (other->GetObjectType() == DynamicEntity)
    {
    }
}
