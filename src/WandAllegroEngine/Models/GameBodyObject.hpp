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

	class GameBodyObject // En caso de hacer solo colisiones sin Gameobject, hacer otra clase llamada "Wall" y heredar de gameobject
	{

	private:
		GameBodyObjectType BodyType;
		int BodyId;
		bool Active;
		Vec2 Position;
		Vec2 Size;
		GameObject *Parent;

	public:
		GameBodyObject() = delete;
		GameBodyObject(GameObject *Parent, Vec2 Position, Vec2 Size);

		void SetPosition(Vec2 NewPosition);
		Vec2 GetPosition();

		void SetSize(Vec2 NewSize);
		Vec2 GetSize();

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