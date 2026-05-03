#ifndef JSON_DEMO_OBJECT_HPP
#define JSON_DEMO_OBJECT_HPP

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/animation/animation_player.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>
#include <glm/glm.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

class JsonDemoObject : public CosmicEngine::Object
{
private:
    std::string label;
    int health;
    float moveSpeed;
    int bodyId;
    CosmicEngine::Timer *directionChangeTimer;
    CosmicEngine::AnimationPlayer *animationPlayer;
    glm::vec2 currentDirection;

    void CreateBody();
    void PickRandomDirection();
    void ResetDirectionTimer();
    void KeepInsideMovementArea();
    void UpdateDirectionalAnimation();
    void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);

    static glm::vec2 movementAreaPosition;
    static glm::vec2 movementAreaSize;

public:
    static const std::string &StaticClassName();
    static void SetMovementArea(glm::vec2 position, glm::vec2 size);

    JsonDemoObject(
        glm::vec2 position = glm::vec2(0.0f),
        glm::vec2 size = glm::vec2(100.0f),
        const std::string &label = "Objeto JSON",
        int health = 100,
        short int layerId = 0
    );
    ~JsonDemoObject() override;

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