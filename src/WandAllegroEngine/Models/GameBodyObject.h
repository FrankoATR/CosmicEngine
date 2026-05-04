#ifndef GAMEBODYOBJECT_H
#define GAMEBODYOBJECT_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>

class GameObject;

struct Vec2{
    float x, y;
    Vec2(float x, float y) : x(x), y(y) {}
};

enum GameBodyObjectType {
	Body_Rectangle,
	Body_Circle,
	Body_Triangle
};

class GameBodyObject  //En caso de hacer solo colisiones sin Gameobject, hacer otra clase llamada "Wall" y heredar de gameobject
{

private:
	GameBodyObjectType BodyType;
	int BodyId;
	bool Active;
    Vec2 Position;
    Vec2 Size;
	GameObject* Parent;

public:
	GameBodyObject(GameObject* Parent, Vec2 Position, Vec2 Size);

	void SetPosition(Vec2 NewPosition);
	Vec2 GetPosition();

	void SetSize(Vec2 NewSize);
	Vec2 GetSize();

	void DrawBody();
	void DrawCoordinates();

    void SetObjectId(int NewBodyId);
    int GetObjectId();

	void SetParent(GameObject* NewParent);
	GameObject* GetParent();

	~GameBodyObject();



};

#endif