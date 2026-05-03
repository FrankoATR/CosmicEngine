#include "sandbox_light_object_3d.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/light/light_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#include <algorithm>
#include <random>
#include <sstream>

namespace
{
    glm::vec3 GenerateRandomDirection3D()
    {
        static std::mt19937 generator(std::random_device{}());
        static std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

        glm::vec3 direction(0.0f);
        do
        {
            direction = glm::vec3(
                distribution(generator),
                distribution(generator),
                distribution(generator));
        } while (glm::length(direction) <= 0.001f);

        return glm::normalize(direction);
    }
}

const std::string &SandboxLightObject3D::StaticClassName()
{
    static const std::string className = "SandboxLightObject3D";
    return className;
}

SandboxLightObject3D::SandboxLightObject3D(
    const std::string &markerTextureKey,
    glm::vec3 position,
    glm::vec3 size,
    glm::vec3 ambientLight,
    glm::vec3 diffuseLight,
    glm::vec3 specularLight,
    CosmicEngine::LightMobility mobility,
    glm::vec3 movementAreaCenter,
    glm::vec3 movementAreaSize,
    float moveSpeed,
    glm::vec3 rotationSpeed)
    : Object(StaticClassName(), position, size, glm::vec3(0.0f)),
      markerTextureKey(markerTextureKey),
      light(nullptr),
      lightId(-1),
      mobility(mobility),
      movementAreaCenter(movementAreaCenter),
      movementAreaSize(movementAreaSize),
      currentDirection(GenerateRandomDirection3D()),
      rotationSpeed(rotationSpeed),
      moveSpeed(moveSpeed)
{
    SetColor(specularLight);

    light = new CosmicEngine::Light(
        position,
        ambientLight,
        diffuseLight,
        specularLight,
        1.0f,
        0.035f,
        0.0012f,
        24.0f,
        true,
        mobility);

    if (mobility == CosmicEngine::LightMobility::Static)
    {
        LGT_MN.AddStatic(light);
    }
    else
    {
        LGT_MN.AddDynamic(light);
    }

    lightId = light->GetID();
}

SandboxLightObject3D::~SandboxLightObject3D()
{
    if (lightId >= 0)
    {
        LGT_MN.Remove(lightId);
        lightId = -1;
        light = nullptr;
    }
}

void SandboxLightObject3D::SyncLightPosition() const
{
    if (light)
    {
        light->SetPosition(GetPosition());
    }
}

void SandboxLightObject3D::PickRandomDirection()
{
    currentDirection = GenerateRandomDirection3D();
}

void SandboxLightObject3D::MoveWithinArea(float deltaTime)
{
    glm::vec3 nextPosition = GetPosition() + currentDirection * moveSpeed * deltaTime;
    const glm::vec3 halfArea = movementAreaSize * 0.5f;
    const glm::vec3 halfSize = GetSize() * 0.5f;
    const glm::vec3 minPosition = movementAreaCenter - halfArea + halfSize;
    const glm::vec3 maxPosition = movementAreaCenter + halfArea - halfSize;

    bool bounced = false;

    if (nextPosition.x < minPosition.x || nextPosition.x > maxPosition.x)
    {
        currentDirection.x *= -1.0f;
        bounced = true;
    }
    if (nextPosition.y < minPosition.y || nextPosition.y > maxPosition.y)
    {
        currentDirection.y *= -1.0f;
        bounced = true;
    }
    if (nextPosition.z < minPosition.z || nextPosition.z > maxPosition.z)
    {
        currentDirection.z *= -1.0f;
        bounced = true;
    }

    if (bounced)
    {
        currentDirection = glm::normalize(currentDirection + 0.35f * GenerateRandomDirection3D());
        nextPosition = GetPosition() + currentDirection * moveSpeed * deltaTime;
    }

    nextPosition = glm::clamp(nextPosition, minPosition, maxPosition);
    SetPosition(nextPosition);
}

void SandboxLightObject3D::draw() const
{
    RS_MN.RenderParallelepipedTextureUnlit(
        markerTextureKey,
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        GetColor(),
        1.0f,
        CosmicEngine::ViewType::Projection);
}

void SandboxLightObject3D::update(float deltaTime)
{
    if (mobility == CosmicEngine::LightMobility::Dynamic && moveSpeed > 0.0f)
    {
        MoveWithinArea(deltaTime);
    }

    SetRotation(GetRotation() + rotationSpeed * deltaTime);
    SyncLightPosition();
}

CosmicEngine::Object *SandboxLightObject3D::Clone() const
{
    auto *clone = new SandboxLightObject3D(
        markerTextureKey,
        GetPosition(),
        GetSize(),
        light ? light->GetAmbientLight() : glm::vec3(0.05f),
        light ? light->GetDiffuseLight() : GetColor(),
        light ? light->GetSpecularLight() : GetColor(),
        mobility,
        movementAreaCenter,
        movementAreaSize,
        moveSpeed,
        rotationSpeed);
    clone->SetColor(GetColor());
    clone->SetRotation(GetRotation());
    return clone;
}

std::vector<std::string> SandboxLightObject3D::GetAllValues() const
{
    const glm::vec3 position = GetPosition();
    return {
        std::string("mobility=") + (mobility == CosmicEngine::LightMobility::Static ? "static" : "dynamic"),
        "position=(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")",
        "speed=" + std::to_string(moveSpeed)
    };
}
