#ifndef GAMEBODYOBJECT_HPP
#define GAMEBODYOBJECT_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <functional>

#include "../Interfaces/Definitions.hpp"

namespace WandEngine
{

	class GameObject;

	class GameBodyObject
	{

	private:
		GameBodyObjectType BodyType;
		int BodyId;
		bool Active;
		WAND_VEC2 Position;
		WAND_VEC2 Size;
		GameObject *Parent;
		std::function<void(GameObject*, CollisionSide)> OnCollisionEvent;
	public:
		GameBodyObject() = delete;
		GameBodyObject(GameObject *Parent, WAND_VEC2 Position, WAND_VEC2 Size, std::function<void(GameObject*, CollisionSide)> OnCollisionEvent);
        void OnCollision(GameBodyObject *other, CollisionSide Side);

		void SetPosition(WAND_VEC2 NewPosition);
		WAND_VEC2 GetPosition();

		void SetSize(WAND_VEC2 NewSize);
		WAND_VEC2 GetSize();

		void DrawBody();
		void DrawCoordinates();

		void SetObjectId(int NewBodyId);
		int GetObjectId();

		void SetParent(GameObject *NewParent);
		GameObject *GetParent();

		~GameBodyObject();
	};
}

#endif // GAMEBODYOBJECT_HPP