#include "collision_demo_object_3d.hpp"

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/body/body_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/managers/timer/timer_manager.hpp>

#include <algorithm>
#include <random>

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

glm::vec3 CollisionDemoObject3D::movementAreaPosition = glm::vec3(-42.0f, -24.0f, -112.0f);
glm::vec3 CollisionDemoObject3D::movementAreaSize = glm::vec3(84.0f, 48.0f, 84.0f);

const std::string &CollisionDemoObject3D::StaticClassName()
{
    static const std::string className = "CollisionDemoObject3D";
    return className;
}

void CollisionDemoObject3D::SetMovementArea(glm::vec3 position, glm::vec3 size)
{
    movementAreaPosition = position;
    movementAreaSize = size;
}

CollisionDemoObject3D::CollisionDemoObject3D(glm::vec3 position, glm::vec3 size, const std::string &label, float moveSpeed)
    : Object(StaticClassName(), position, size, glm::vec3(0.0f)),
      label(label),
      moveSpeed(moveSpeed),
      bodyId(-1),
      directionChangeTimer(nullptr),
      currentDirection(glm::vec3(0.0f, 0.0f, -1.0f))
{
    CreateBody();
    PickRandomDirection();
    ResetDirectionTimer();
}

CollisionDemoObject3D::~CollisionDemoObject3D()
{
    if (bodyId >= 0)
    {
        BOD_MN.Remove(bodyId);
        bodyId = -1;
    }

    if (directionChangeTimer)
    {
        directionChangeTimer->Destroy();
        directionChangeTimer = nullptr;
    }
}

void CollisionDemoObject3D::CreateBody()
{
    CosmicEngine::Body *body = new CosmicEngine::Body(
        this,
        glm::vec3(0.0f),
        GetSize(),
        CALLBACK_COLLISION_EVENT(OnBodyCollision));

    BOD_MN.Add(body);
    bodyId = body->GetID();
}

void CollisionDemoObject3D::PickRandomDirection()
{
    currentDirection = GenerateRandomDirection3D();
}

void CollisionDemoObject3D::ResetDirectionTimer()
{
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<double> timerDistribution(0.8, 2.1);

    double nextDirectionTime = timerDistribution(generator);

    if (!directionChangeTimer)
    {
        directionChangeTimer = new CosmicEngine::Timer(nextDirectionTime, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(directionChangeTimer);
        return;
    }

    directionChangeTimer->SetTargetTime(nextDirectionTime);
    directionChangeTimer->reset();
    directionChangeTimer->Play();
}

void CollisionDemoObject3D::KeepInsideMovementArea()
{
    glm::vec3 position = GetPosition();
    glm::vec3 size = GetSize();
    glm::vec3 minPosition = movementAreaPosition;
    glm::vec3 maxPosition = movementAreaPosition + movementAreaSize - size;

    bool changedDirection = false;

    if (position.x < minPosition.x)
    {
        position.x = minPosition.x;
        currentDirection.x = std::abs(currentDirection.x);
        changedDirection = true;
    }
    else if (position.x > maxPosition.x)
    {
        position.x = maxPosition.x;
        currentDirection.x = -std::abs(currentDirection.x);
        changedDirection = true;
    }

    if (position.y < minPosition.y)
    {
        position.y = minPosition.y;
        currentDirection.y = std::abs(currentDirection.y);
        changedDirection = true;
    }
    else if (position.y > maxPosition.y)
    {
        position.y = maxPosition.y;
        currentDirection.y = -std::abs(currentDirection.y);
        changedDirection = true;
    }

    if (position.z < minPosition.z)
    {
        position.z = minPosition.z;
        currentDirection.z = std::abs(currentDirection.z);
        changedDirection = true;
    }
    else if (position.z > maxPosition.z)
    {
        position.z = maxPosition.z;
        currentDirection.z = -std::abs(currentDirection.z);
        changedDirection = true;
    }

    if (changedDirection)
    {
        SetPosition(position);
    }
}

void CollisionDemoObject3D::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
{
    (void)side;

    if (!other)
    {
        return;
    }

    if (other->GetClassName() == StaticClassName())
    {
        Destroy();
        other->Destroy();
    }
}

void CollisionDemoObject3D::draw() const
{
    RS_MN.RenderParallelepipedLines(
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        GetColor(),
        1.0f,
        3.0f,
        false,
        CosmicEngine::ViewType::Projection);
}

void CollisionDemoObject3D::update(float deltaTime)
{
    (void)deltaTime;

    if (directionChangeTimer && directionChangeTimer->IsTrigger())
    {
        PickRandomDirection();
        ResetDirectionTimer();
    }

    SetVelocity(currentDirection * moveSpeed);
    KeepInsideMovementArea();
}

CosmicEngine::Object *CollisionDemoObject3D::Clone() const
{
    CollisionDemoObject3D *clone = new CollisionDemoObject3D(GetPosition(), GetSize(), label, moveSpeed);
    clone->SetColor(GetColor());
    clone->SetRotation(GetRotation());
    return clone;
}

std::vector<std::string> CollisionDemoObject3D::GetAllValues() const
{
    glm::vec3 position = GetPosition();
    glm::vec3 size = GetSize();

    return {
        "label=" + label,
        "position=(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")",
        "size=(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ", " + std::to_string(size.z) + ")",
        "speed=" + std::to_string(moveSpeed)
    };
}

#endif