#ifndef SANDBOX_LIGHT_OBJECT_3D_HPP
#define SANDBOX_LIGHT_OBJECT_3D_HPP

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/light/light.hpp>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class SandboxLightObject3D : public CosmicEngine::Object
{
public:
    static const std::string &StaticClassName();

    SandboxLightObject3D(
        const std::string &markerTextureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 ambientLight,
        glm::vec3 diffuseLight,
        glm::vec3 specularLight,
        CosmicEngine::LightMobility mobility,
        glm::vec3 movementAreaCenter,
        glm::vec3 movementAreaSize,
        float moveSpeed = 0.0f,
        glm::vec3 rotationSpeed = glm::vec3(0.0f));

    ~SandboxLightObject3D() override;

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;
    std::vector<std::string> GetAllValues() const override;

private:
    void SyncLightPosition() const;
    void PickRandomDirection();
    void MoveWithinArea(float deltaTime);

    std::string markerTextureKey;
    CosmicEngine::Light *light;
    int lightId;
    CosmicEngine::LightMobility mobility;
    glm::vec3 movementAreaCenter;
    glm::vec3 movementAreaSize;
    glm::vec3 currentDirection;
    glm::vec3 rotationSpeed;
    float moveSpeed;
};

#endif
