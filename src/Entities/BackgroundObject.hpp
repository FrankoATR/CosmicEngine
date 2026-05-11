#ifndef BACKGROUNDOBJECT_HPP
#define BACKGROUNDOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

class BackgroundObject : public GameObject
{
public:
    BackgroundObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Update(float deltaTime) override;

};



#endif // BACKGROUNDOBJECT_HPP