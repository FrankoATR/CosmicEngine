#include "sandbox_rotating_cube_object_3d.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#include <sstream>

const std::string &SandboxRotatingCubeObject3D::StaticClassName()
{
    static const std::string className = "SandboxRotatingCubeObject3D";
    return className;
}

SandboxRotatingCubeObject3D::SandboxRotatingCubeObject3D(
    const std::string &textureKey,
    glm::vec3 position,
    glm::vec3 size,
    glm::vec3 rotation,
    glm::vec3 rotationSpeed,
    glm::vec3 outlineColor)
    : Object(StaticClassName(), position, size, rotation),
      textureKey(textureKey),
      rotationSpeed(rotationSpeed),
      outlineColor(outlineColor)
{
}

void SandboxRotatingCubeObject3D::draw() const
{
    RS_MN.RenderParallelepipedTextureAtlasLit(
        textureKey,
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        GetColor(),
        1.0f,
        CosmicEngine::ViewType::Projection);

    RS_MN.RenderParallelepipedLines(
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        outlineColor,
        0.95f,
        2.2f,
        false,
        CosmicEngine::ViewType::Projection);
}

void SandboxRotatingCubeObject3D::update(float deltaTime)
{
    SetRotation(GetRotation() + rotationSpeed * deltaTime);
}

CosmicEngine::Object *SandboxRotatingCubeObject3D::Clone() const
{
    auto *clone = new SandboxRotatingCubeObject3D(textureKey, GetPosition(), GetSize(), GetRotation(), rotationSpeed, outlineColor);
    clone->SetColor(GetColor());
    return clone;
}

std::vector<std::string> SandboxRotatingCubeObject3D::GetAllValues() const
{
    const glm::vec3 position = GetPosition();
    const glm::vec3 size = GetSize();
    const glm::vec3 rotation = GetRotation();
    std::ostringstream rotationStream;
    rotationStream << rotation.x << ", " << rotation.y << ", " << rotation.z;
    return {
        "texture=" + textureKey,
        "position=(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")",
        "size=(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ", " + std::to_string(size.z) + ")",
        "rotation=(" + rotationStream.str() + ")"
    };
}
