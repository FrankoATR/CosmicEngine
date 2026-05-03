#ifndef SANDBOX_ROTATING_CUBE_OBJECT_3D_HPP
#define SANDBOX_ROTATING_CUBE_OBJECT_3D_HPP

#include <CosmicEngine/models/object/object.hpp>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class SandboxRotatingCubeObject3D : public CosmicEngine::Object
{
public:
    static const std::string &StaticClassName();

    SandboxRotatingCubeObject3D(
        const std::string &textureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 rotation = glm::vec3(0.0f),
        glm::vec3 rotationSpeed = glm::vec3(0.0f, 8.0f, 0.0f),
        glm::vec3 outlineColor = glm::vec3(1.0f, 0.45f, 0.15f));

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;
    std::vector<std::string> GetAllValues() const override;

private:
    std::string textureKey;
    glm::vec3 rotationSpeed;
    glm::vec3 outlineColor;
};

#endif
