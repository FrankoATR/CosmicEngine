#ifndef MAPTILEOBJECT_HPP
#define MAPTILEOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

class MapTileObject : public GameObject
{
public:
    MapTileObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;

    ~MapTileObject() override;
};



#endif // MAPTILEOBJECT_HPP