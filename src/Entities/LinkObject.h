#ifndef LINKOBJECT_H
#define LINKOBJECT_H

#include "../WandAllegroEngine/Models/GameObject.h"

class LinkObject : public GameObject
{
private:

    //std::vector<GameObject*> inventory;
    ALLEGRO_FONT* font;
    int HP;

public:
    LinkObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font);
    void Draw() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);

};



#endif