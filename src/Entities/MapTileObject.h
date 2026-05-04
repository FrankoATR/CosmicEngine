#ifndef MAPTILEOBJECT_H
#define MAPTILEOBJECT_H

#include "../WandAllegroEngine/Models/GameObject.h"

class MapTileObject : public GameObject
{
public:
    MapTileObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Update(float deltaTime) override;

    ~MapTileObject() override;
};



#endif