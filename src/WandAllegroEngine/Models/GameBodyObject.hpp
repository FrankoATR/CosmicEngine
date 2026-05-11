#ifndef GAMEBODYOBJECT_HPP
#define GAMEBODYOBJECT_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
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

	public:
		GameBodyObject() = delete;
		GameBodyObject(GameObject *Parent, WAND_VEC2 Position, WAND_VEC2 Size);

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