#ifndef SANDBOX_HAMMER_OBJECT_3D_HPP
#define SANDBOX_HAMMER_OBJECT_3D_HPP

#include <CosmicEngine/models/object/object.hpp>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class SandboxHammerObject3D : public CosmicEngine::Object
{
public:
    static const std::string &StaticClassName();

    SandboxHammerObject3D(
        const std::string &modelKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 rotation = glm::vec3(0.0f),
        glm::vec3 rotationSpeed = glm::vec3(0.0f, 28.0f, 0.0f));

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;
    std::vector<std::string> GetAllValues() const override;

private:
    std::string modelKey;
    glm::vec3 rotationSpeed;
};

#endif
