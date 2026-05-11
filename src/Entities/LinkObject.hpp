#ifndef LINKOBJECT_HPP
#define LINKOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"
#include "../WandAllegroEngine/Models/GameTimer.hpp"

using namespace WandEngine;

class LinkObject : public GameObject
{
private:
    // std::vector<GameObject*> inventory;
    ALLEGRO_FONT *font;
    int HP;
    bool DestructorMode;
    bool IsTPActive;
    float Velocity;
    GameTimer* WaitShootTimer;

public:
    LinkObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font);
    ~LinkObject();
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject *other) override;

    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);
    
    void MoveUpLeft(float deltaTime);
    void MoveUpRight(float deltaTime);
    void MoveDownLeft(float deltaTime);
    void MoveDownRight(float deltaTime);


};


#endif // LINKOBJECT_HPP