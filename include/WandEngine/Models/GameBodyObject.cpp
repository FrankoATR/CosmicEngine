#include "GameBodyObject.hpp"

#include "../Managers/Resource/ResourceManager.hpp"
#include "GameObject.hpp"

#include <iostream>

namespace WandEngine
{
	GameBodyObject::GameBodyObject(GameObject *Parent, glm::vec2 OffSetParentPosition, glm::vec2 Size, std::function<void(GameObject*, CollisionSide)> OnCollisionEvent) : 
		Parent(Parent), OffSetParentPosition(OffSetParentPosition), Size(Size), Active(true), OnCollisionEvent(OnCollisionEvent), AliveInGameManager(true)
	{
		if(!Parent)
		{
			std::cerr << "Parent NOT FOUND for this Body" << std::endl;
		}
		if(!OnCollisionEvent)
		{
			std::cerr << "OnCollisionEvent NOT FOUND for this Body" << std::endl;
		}
		Position = Parent->GetPosition() + OffSetParentPosition;
	}

    void GameBodyObject::OnCollision(GameBodyObject *other, CollisionSide Side)
    {
		if(OnCollisionEvent && Parent)
		{
			OnCollisionEvent(other->GetParent(), Side);
		}
    }

	void GameBodyObject::UpdatePosition()
	{
		Position = Parent->GetPosition() + OffSetParentPosition;
	}

	glm::vec2 GameBodyObject::GetPosition()
	{
		return this->Position;
	}


	void GameBodyObject::SetOffSetParentPosition(glm::vec2 NewOffSetParentPosition)
	{
		this->OffSetParentPosition = NewOffSetParentPosition;
	}

	glm::vec2 GameBodyObject::GetOffSetParentPosition()
	{
		return this->OffSetParentPosition;
	}

	void GameBodyObject::SetSize(glm::vec2 NewSize)
	{
		this->Size = NewSize;
	}

	glm::vec2 GameBodyObject::GetSize()
	{
		return this->Size;
	}

	void GameBodyObject::DrawBody()
	{
		ResourceManager::GetInstance().RenderRectangle(
			glm::vec3(Position, 0.0f), 
			glm::vec3(Position + Size, 0.0f), 
			glm::vec3(Position.x + Size.x / 2, Position.y + Size.y / 2, 0.0f), 
			glm::vec3(0.0f, 0.0f, Rotation), 
			glm::vec3(1.0f, 0.0f, 0.0f), 
			1.0f, 
        	2.0f);
	}

	void GameBodyObject::DrawCoordinates()
	{

	}

	void GameBodyObject::SetID(int NewBodyId)
	{
		this->ID = NewBodyId;
	}

	int GameBodyObject::GetID()
	{
		return this->ID;
	}

	void GameBodyObject::SetParent(GameObject *NewParent)
	{
		this->Parent = NewParent;
	}

	GameObject *GameBodyObject::GetParent()
	{
		return this->Parent;
	}

	void GameBodyObject::SetOnCollisionEvent(std::function<void(GameObject*, CollisionSide)> newOnCollionEvent)
	{
		OnCollisionEvent = newOnCollionEvent;
	}

	std::function<void(GameObject*, CollisionSide)> GameBodyObject::GetOnCollisionEvent()
	{
		return OnCollisionEvent;
	}


    void GameBodyObject::Destroy()
    {
        this->AliveInGameManager = false;
    }

	bool GameBodyObject::GetAliveInGameManager() const
    {
        return this->AliveInGameManager;
    }

	GameBodyObject::~GameBodyObject()
	{

	}

}