#include "network_demo_player.hpp"

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/body/body_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/managers/input/input_manager.hpp>
#include <CosmicEngine/managers/camera/camera_manager.hpp>

#include <algorithm>
#include <iostream>

namespace
{
    constexpr float kPlayerGravity = -42.0f;
    constexpr float kPlayerJumpVelocity = 16.0f;
    constexpr float kPlayerMaxFallSpeed = -35.0f;
}

glm::vec3 NetworkDemoPlayer::movementAreaPosition = glm::vec3(-30.0f, -2.0f, -30.0f);
glm::vec3 NetworkDemoPlayer::movementAreaSize = glm::vec3(60.0f, 20.0f, 60.0f);

const std::string &NetworkDemoPlayer::StaticClassName()
{
    static const std::string className = "NetworkDemoPlayer";
    return className;
}

void NetworkDemoPlayer::SetMovementArea(glm::vec3 position, glm::vec3 size)
{
    movementAreaPosition = position;
    movementAreaSize = size;
}

NetworkDemoPlayer::NetworkDemoPlayer(int networkId, const std::string &playerName, glm::vec3 position, glm::vec3 size, bool isLocal)
    : Object(StaticClassName(), position, size, glm::vec3(0.0f)),
      networkId(networkId),
      playerName(playerName),
      health(20),
      maxHealth(20),
      moveSpeed(15.0f),
      bodyId(-1),
            isLocal(isLocal),
            grounded(false)
{
    CreateBody();
        ClampToMovementArea();
        grounded = GetPosition().y <= movementAreaPosition.y + 0.001f;

    // Color: green for local, red for remote
    if (isLocal)
        SetColor(glm::vec3(0.2f, 0.9f, 0.3f));
    else
        SetColor(glm::vec3(0.9f, 0.2f, 0.2f));
}

NetworkDemoPlayer::~NetworkDemoPlayer()
{
    if (bodyId >= 0)
    {
        BOD_MN.Remove(bodyId);
        bodyId = -1;
    }
}

void NetworkDemoPlayer::CreateBody()
{
    CosmicEngine::Body *body = new CosmicEngine::Body(
        this,
        glm::vec3(0.0f),
        GetSize(),
        CALLBACK_COLLISION_EVENT(OnBodyCollision));

    BOD_MN.Add(body);
    bodyId = body->GetID();
}

void NetworkDemoPlayer::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
{
    (void)side;
    (void)other;
    // Collision handling is done in the scene via network messages
}

void NetworkDemoPlayer::ClampToMovementArea()
{
    glm::vec3 position = GetPosition();
    glm::vec3 velocity = GetVelocity();
    glm::vec3 maxPosition = movementAreaPosition + movementAreaSize - GetSize();

    position.x = std::clamp(position.x, movementAreaPosition.x, maxPosition.x);
    position.z = std::clamp(position.z, movementAreaPosition.z, maxPosition.z);

    if (position.y <= movementAreaPosition.y)
    {
        position.y = movementAreaPosition.y;
        velocity.y = std::max(0.0f, velocity.y);
        grounded = true;
    }
    else
    {
        grounded = false;
    }

    if (position.y > maxPosition.y)
    {
        position.y = maxPosition.y;
        velocity.y = std::min(0.0f, velocity.y);
    }

    SetPosition(position);
    SetVelocity(velocity);
}

void NetworkDemoPlayer::draw() const
{
    glm::vec3 color = GetColor();

    // Draw player as a filled parallelepiped
    RS_MN.RenderParallelepipedLines(
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        GetRotation(),
        color,
        1.0f,
        3.0f,
        false,
        CosmicEngine::ViewType::Projection);

    // Draw health bar above the player
    float healthRatio = static_cast<float>(health) / static_cast<float>(maxHealth);
    float barWidth = GetSize().x * 1.5f;
    float barHeight = 0.3f;
    glm::vec3 barPos = GetPosition() + glm::vec3(-barWidth * 0.25f, GetSize().y + 0.5f, 0.0f);

    // Background (red)
    RS_MN.RenderParallelepipedLines(
        barPos,
        glm::vec3(barWidth, barHeight, 0.2f),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f),
        0.8f,
        1.0f,
        false,
        CosmicEngine::ViewType::Projection);

    // Health (green)
    if (healthRatio > 0.0f)
    {
        RS_MN.RenderParallelepipedLines(
            barPos,
            glm::vec3(barWidth * healthRatio, barHeight, 0.2f),
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 0.9f, 0.0f),
            0.8f,
            1.0f,
            false,
            CosmicEngine::ViewType::Projection);
    }
}

void NetworkDemoPlayer::update(float deltaTime)
{
    if (!isLocal)
    {
        ClampToMovementArea();
        return;
    }

    auto &input = INP_MN;
    glm::vec3 velocity = GetVelocity();
    glm::vec3 horizontalVelocity(0.0f);

    if (input.IsKeyPressed(GLFW_KEY_W, CosmicEngine::KeyRelease))
        horizontalVelocity.z -= 1.0f;
    if (input.IsKeyPressed(GLFW_KEY_S, CosmicEngine::KeyRelease))
        horizontalVelocity.z += 1.0f;
    if (input.IsKeyPressed(GLFW_KEY_A, CosmicEngine::KeyRelease))
        horizontalVelocity.x -= 1.0f;
    if (input.IsKeyPressed(GLFW_KEY_D, CosmicEngine::KeyRelease))
        horizontalVelocity.x += 1.0f;

    if (glm::length(horizontalVelocity) > 0.001f)
    {
        horizontalVelocity = glm::normalize(horizontalVelocity) * moveSpeed;
    }
    velocity.x = horizontalVelocity.x;
    velocity.z = horizontalVelocity.z;

    if (grounded && input.IsKeyPressed(GLFW_KEY_SPACE, CosmicEngine::KeyDown))
    {
        velocity.y = kPlayerJumpVelocity;
        grounded = false;
    }

    if (!grounded)
    {
        velocity.y += kPlayerGravity * deltaTime;
        velocity.y = std::max(velocity.y, kPlayerMaxFallSpeed);
    }

    glm::vec3 nextPosition = GetPosition() + velocity * deltaTime;
    glm::vec3 maxPosition = movementAreaPosition + movementAreaSize - GetSize();

    if (nextPosition.x < movementAreaPosition.x || nextPosition.x > maxPosition.x)
    {
        velocity.x = 0.0f;
    }

    if (nextPosition.z < movementAreaPosition.z || nextPosition.z > maxPosition.z)
    {
        velocity.z = 0.0f;
    }

    if (nextPosition.y <= movementAreaPosition.y)
    {
        velocity.y = 0.0f;
        grounded = true;
        SetPosition(glm::vec3(GetPosition().x, movementAreaPosition.y, GetPosition().z));
    }
    else if (nextPosition.y > maxPosition.y)
    {
        velocity.y = 0.0f;
    }

    SetVelocity(velocity);
    ClampToMovementArea();
}

CosmicEngine::Object *NetworkDemoPlayer::Clone() const
{
    auto *clone = new NetworkDemoPlayer(networkId, playerName, GetPosition(), GetSize(), isLocal);
    clone->SetColor(GetColor());
    clone->SetHealth(health);
    return clone;
}

int NetworkDemoPlayer::GetNetworkId() const { return networkId; }
const std::string &NetworkDemoPlayer::GetPlayerName() const { return playerName; }

void NetworkDemoPlayer::SetHealth(int h) { health = std::clamp(h, 0, maxHealth); }
int NetworkDemoPlayer::GetHealth() const { return health; }
int NetworkDemoPlayer::GetMaxHealth() const { return maxHealth; }
bool NetworkDemoPlayer::IsAlivePlayer() const { return health > 0; }

void NetworkDemoPlayer::TakeDamage(int amount)
{
    health = std::max(0, health - amount);
}

void NetworkDemoPlayer::SetIsLocal(bool local)
{
    isLocal = local;
    if (isLocal)
        SetColor(glm::vec3(0.2f, 0.9f, 0.3f));
    else
        SetColor(glm::vec3(0.9f, 0.2f, 0.2f));
}

bool NetworkDemoPlayer::GetIsLocal() const { return isLocal; }

bool NetworkDemoPlayer::IsGrounded() const { return grounded; }

float NetworkDemoPlayer::GetMoveSpeed() const { return moveSpeed; }

#endif // GAME_3D_CONFIGURATION
