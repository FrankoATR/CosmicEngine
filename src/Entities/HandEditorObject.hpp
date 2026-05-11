#ifndef HANDEDITOROBJECT_HPP
#define HANDEDITOROBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

using namespace WandEngine;

class HandEditorObject : public GameObject
{
public:
    HandEditorObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;

    ~HandEditorObject() override;
};



#endif // HANDEDITOROBJECT_HPP