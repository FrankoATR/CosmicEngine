#include "GameBodyObject.hpp"

namespace WandEngine
{
	GameBodyObject::GameBodyObject(GameObject *Parent, WAND_VEC2 Position, WAND_VEC2 Size, std::function<void(GameObject*, CollisionSide)> OnCollisionEvent) : 
		Parent(Parent), Position(Position), Size(Size), Active(true), OnCollisionEvent(OnCollisionEvent)
	{
		// this->BodyType = BodyType;
	}

    void GameBodyObject::OnCollision(GameBodyObject *other, CollisionSide Side)
    {
		if(OnCollisionEvent)
		{
			OnCollisionEvent(other->GetParent(), Side);
		}
    }

	void GameBodyObject::SetPosition(WAND_VEC2 NewPosition)
	{
		this->Position = NewPosition;
	}

	WAND_VEC2 GameBodyObject::GetPosition()
	{
		return this->Position;
	}

	void GameBodyObject::SetSize(WAND_VEC2 NewSize)
	{
		this->Size = NewSize;
	}

	WAND_VEC2 GameBodyObject::GetSize()
	{
		return this->Size;
	}

	void GameBodyObject::DrawBody()
	{
		al_draw_rectangle(this->Position.x, this->Position.y, this->Position.x + this->Size.x, this->Position.y + this->Size.y, al_map_rgba(255, 255, 255, 1), 1);
	}

	void GameBodyObject::DrawCoordinates()
	{
	}

	void GameBodyObject::SetObjectId(int NewBodyId)
	{
		this->BodyId = NewBodyId;
	}

	int GameBodyObject::GetObjectId()
	{
		return this->BodyId;
	}

	void GameBodyObject::SetParent(GameObject *NewParent)
	{
		this->Parent = NewParent;
	}

	GameObject *GameBodyObject::GetParent()
	{
		return this->Parent;
	}

	GameBodyObject::~GameBodyObject()
	{
	}

}