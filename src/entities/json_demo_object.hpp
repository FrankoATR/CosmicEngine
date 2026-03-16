#ifndef JSON_DEMO_OBJECT_HPP
#define JSON_DEMO_OBJECT_HPP

#include <CosmicEngine/models/object/object.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

class JsonDemoObject : public CosmicEngine::Object
{
private:
    std::string label;
    int health;
    float moveSpeed;

public:
    static const std::string &StaticClassName();

    JsonDemoObject(
        glm::vec2 position = glm::vec2(0.0f),
        glm::vec2 size = glm::vec2(100.0f),
        const std::string &label = "Objeto JSON",
        int health = 100,
        short int layerId = 0
    );

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;
    std::vector<std::string> GetAllValues() const override;

    void SetLabel(const std::string &newLabel);
    const std::string &GetLabel() const;

    void SetHealth(int newHealth);
    int GetHealth() const;

    static void RegisterJsonSerialization();
    static nlohmann::json Serialize(const CosmicEngine::Object *object);
    static CosmicEngine::Object *Deserialize(const nlohmann::json &data);
};

#endif // JSON_DEMO_OBJECT_HPP