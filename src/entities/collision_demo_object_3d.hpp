#ifndef COLLISION_DEMO_OBJECT_3D_HPP
#define COLLISION_DEMO_OBJECT_3D_HPP

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class CollisionDemoObject3D : public CosmicEngine::Object
{
private:
    std::string label;
    float moveSpeed;
    int bodyId;
    CosmicEngine::Timer *directionChangeTimer;
    glm::vec3 currentDirection;

    void CreateBody();
    void PickRandomDirection();
    void ResetDirectionTimer();
    void KeepInsideMovementArea();
    void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);

    static glm::vec3 movementAreaPosition;
    static glm::vec3 movementAreaSize;

public:
    static const std::string &StaticClassName();
    static void SetMovementArea(glm::vec3 position, glm::vec3 size);

    CollisionDemoObject3D(
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 size = glm::vec3(8.0f),
        const std::string &label = "Collision 3D",
        float moveSpeed = 18.0f);
    ~CollisionDemoObject3D() override;

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;
    std::vector<std::string> GetAllValues() const override;
};

#endif

#endif // COLLISION_DEMO_OBJECT_3D_HPP