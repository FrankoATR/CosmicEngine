#ifndef LINKOBJECT_HPP
#define LINKOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

class LinkObject : public GameObject
{
private:

    //std::vector<GameObject*> inventory;
    ALLEGRO_FONT* font;
    int HP;
    bool DestructorMode;
    bool IsTPActive;
    int TimeToTP;
    double last_time;

public:
    LinkObject(Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);

};



#endif // LINKOBJECT_HPP