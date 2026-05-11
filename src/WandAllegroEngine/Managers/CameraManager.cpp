#include "CameraManager.hpp"
#include "GameManager.hpp"
#include "../Models/GameObject.hpp"
#include "BodyManager.hpp"

namespace WandEngine
{

    CameraManager::CameraManager() : Position(WAND_VEC2(0, 0))
    {
        Reset();
    }


    WAND_VEC2 CameraManager::GetGridPosition()
    {
        return BodyManager::GetInstance().GetGridPosition();
    }
    void CameraManager::SetGridPosition(WAND_VEC2 NewPosition)
    {
        return BodyManager::GetInstance().SetGridPosition(NewPosition);
    }

    void CameraManager::Reset()
    {
        this->Position = WAND_VEC2(0.0f, 0.0f);
        this->Size = GameManager::GetInstance().GetWindowsSize();
        Transform();
    }

    void CameraManager::Transform()
    {
        SetGridPosition(Position);
        al_identity_transform(&camera);
        al_translate_transform(&camera, -this->Position.x, -this->Position.y);
        al_use_transform(&camera);
    }

    WAND_VEC2 CameraManager::GetPosition()
    {
       return  this->Position;
    }

    void CameraManager::Draw()
    {
        al_draw_line(Position.x , Position.y + Size.height/2, Position.x + Size.width, Position.y + Size.height/2, al_map_rgba(255, 255, 255, 255), 1);

        al_draw_line(Position.x + Size.width/2 , Position.y, Position.x + Size.width/2, Position.y + Size.height, al_map_rgba(255, 255, 255, 255), 1);
    }

    void CameraManager::FocusObject(GameObject* Obj){
        this->Position.x = -(Size.width / 2) + (Obj->GetPosition().x + Obj->GetSize().x / 2);
        this->Position.y = -(Size.height / 2) + (Obj->GetPosition().y + Obj->GetSize().y / 2);
        Transform();
    }

    void CameraManager::FocusPosition(WAND_VEC2 NewPosition){
        this->Position.x = -(Size.width / 2) + (NewPosition.x);
        this->Position.y = -(Size.height / 2) + (NewPosition.y);
        Transform();
    }

}