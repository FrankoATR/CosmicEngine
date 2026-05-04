#ifndef BACKGROUNDOBJECT_H
#define BACKGROUNDOBJECT_H

#include "../WandAllegroEngine/Models/GameObject.h"

class BackgroundObject : public GameObject
{
public:
    BackgroundObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Update(float deltaTime) override;

};



#endif