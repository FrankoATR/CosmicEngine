#include "GameBodyObject.h"


GameBodyObject::GameBodyObject(GameObject* Parent, Vec2 Position, Vec2 Size) : Parent(Parent), Position(Position), Size(Size), Active(true)
{
	//this->BodyType = BodyType;
}


void GameBodyObject::SetPosition(Vec2 NewPosition){
	this->Position = NewPosition;
}


Vec2 GameBodyObject::GetPosition(){
	return this->Position;
}



void GameBodyObject::SetSize(Vec2 NewSize){
	this->Size = NewSize;
}


Vec2 GameBodyObject::GetSize(){
	return this->Size;
}



void GameBodyObject::DrawBody(){
    al_draw_rectangle(this->Position.x, this->Position.y, this->Position.x + this->Size.x, this->Position.y + this->Size.y, al_map_rgba(255, 255, 255, 1), 1);
}


void GameBodyObject::DrawCoordinates(){
	
}

void GameBodyObject::SetObjectId(int NewBodyId){
	this->BodyId = NewBodyId;
}


int GameBodyObject::GetObjectId(){
	return this->BodyId;	
}


void GameBodyObject::SetParent(GameObject* NewParent){
	this->Parent = NewParent;
}


GameObject* GameBodyObject::GetParent(){
	return this->Parent;
}


GameBodyObject::~GameBodyObject()
{
	
}

