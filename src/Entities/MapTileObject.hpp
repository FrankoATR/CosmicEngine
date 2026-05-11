#ifndef MAPTILEOBJECT_HPP
#define MAPTILEOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

using namespace WandEngine;

class MapTileObject : public GameObject
{
public:
    MapTileObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;

    ~MapTileObject() override;
};



#endif // MAPTILEOBJECT_HPP