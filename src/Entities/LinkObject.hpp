#ifndef LINKOBJECT_HPP
#define LINKOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

using namespace WandEngine;

class LinkObject : public GameObject
{
private:
    // std::vector<GameObject*> inventory;
    ALLEGRO_FONT *font;
    int HP;
    bool DestructorMode;
    bool IsTPActive;
    int TimeToTP;
    double last_time;

public:
    LinkObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, int HP, ALLEGRO_FONT *font);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject *other) override;
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);


};



namespace nlohmann
{
    template <>
    struct adl_serializer<LinkObject>
    {
    static void to_json(nlohmann::json &j, const GameObject &obj)
    {
        j = nlohmann::json{
            {"UniqueName", obj.GetObjectName()},
            {"SpriteId", 0},
            {"Position", obj.GetPosition()},
            {"Size", obj.GetSize()}};
    }

    static void from_json(const nlohmann::json &j, GameObject &obj)
    {
        std::string uniqueName;
        j.at("UniqueName").get_to(uniqueName);
        obj.SetObjectName(uniqueName);

        int spriteId;
        j.at("SpriteId").get_to(spriteId);
        // obj.SetSpriteId(spriteId);

        WAND_VEC2 position;
        j.at("Position").get_to(position);
        obj.SetPosition(position);

        WAND_VEC2 size;
        j.at("Size").get_to(size);
        obj.SetSize(size);
    }
    };
}

#endif // LINKOBJECT_HPP