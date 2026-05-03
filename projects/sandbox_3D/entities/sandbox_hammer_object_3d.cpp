#include "sandbox_hammer_object_3d.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#include <sstream>

const std::string &SandboxHammerObject3D::StaticClassName()
{
    static const std::string className = "SandboxHammerObject3D";
    return className;
}

SandboxHammerObject3D::SandboxHammerObject3D(
    const std::string &modelKey,
    glm::vec3 position,
    glm::vec3 size,
    glm::vec3 rotation,
    glm::vec3 rotationSpeed)
    : Object(StaticClassName(), position, size, rotation),
      modelKey(modelKey),
      rotationSpeed(rotationSpeed)
{
}

void SandboxHammerObject3D::draw() const
{
    RS_MN.Render3DModelLit(
        modelKey,
        GetPosition(),
        GetSize(),
        GetRotation(),
        GetColor(),
        1.0f,
        CosmicEngine::ViewType::Projection);

    RS_MN.RenderParallelepipedLines(
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        GetColor(),
        0.85f,
        1.6f,
        false,
        CosmicEngine::ViewType::Projection);
}

void SandboxHammerObject3D::update(float deltaTime)
{
    SetRotation(GetRotation() + rotationSpeed * deltaTime);
}

CosmicEngine::Object *SandboxHammerObject3D::Clone() const
{
    auto *clone = new SandboxHammerObject3D(modelKey, GetPosition(), GetSize(), GetRotation(), rotationSpeed);
    clone->SetColor(GetColor());
    return clone;
}

std::vector<std::string> SandboxHammerObject3D::GetAllValues() const
{
    const glm::vec3 position = GetPosition();
    const glm::vec3 size = GetSize();
    const glm::vec3 rotation = GetRotation();
    std::ostringstream rotationStream;
    rotationStream << rotation.x << ", " << rotation.y << ", " << rotation.z;
    return {
        "model=" + modelKey,
        "position=(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")",
        "size=(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ", " + std::to_string(size.z) + ")",
        "rotation=(" + rotationStream.str() + ")"
    };
}
