#include "network_demo_projectile.hpp"

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/body/body_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>

#include <cmath>

const std::string &NetworkDemoProjectile::StaticClassName()
{
    static const std::string className = "NetworkDemoProjectile";
    return className;
}

NetworkDemoProjectile::NetworkDemoProjectile(int ownerNetworkId, int networkObjectId, int damage,
                                             glm::vec3 position, glm::vec3 size, float maxLifetime)
    : Object(StaticClassName(), position, size, glm::vec3(0.0f)),
      ownerNetworkId(ownerNetworkId),
      damage(damage),
      lifetime(0.0f),
      maxLifetime(maxLifetime),
      bodyId(-1),
      networkObjectId(networkObjectId)
{
    // Color based on damage: brighter = more damage
    float intensity = 0.3f + (static_cast<float>(damage) / 3.0f) * 0.7f;
    SetColor(glm::vec3(intensity, 0.2f, 1.0f - intensity * 0.5f));

    CreateBody();
}

NetworkDemoProjectile::~NetworkDemoProjectile()
{
    if (bodyId >= 0)
    {
        BOD_MN.Remove(bodyId);
        bodyId = -1;
    }
}

void NetworkDemoProjectile::CreateBody()
{
    CosmicEngine::Body *body = new CosmicEngine::Body(
        this,
        glm::vec3(0.0f),
        GetSize(),
        nullptr);

    BOD_MN.Add(body);
    bodyId = body->GetID();
}

void NetworkDemoProjectile::draw() const
{
    // Pulse effect based on lifetime
    float pulse = 0.7f + 0.3f * std::sin(lifetime * 4.0f);
    glm::vec3 color = GetColor() * pulse;

    // Fade out in the last second
    float alpha = 1.0f;
    float remaining = maxLifetime - lifetime;
    if (remaining < 1.0f)
    {
        alpha = remaining;
    }

    RS_MN.RenderParallelepipedLines(
        GetPosition(),
        GetSize(),
        glm::vec3(0.0f),
        glm::vec3(0.0f, lifetime * 45.0f, 0.0f), // Spin for visual effect
        color,
        alpha,
        2.5f,
        false,
        CosmicEngine::ViewType::Projection);
}

void NetworkDemoProjectile::update(float deltaTime)
{
    lifetime += deltaTime;

    if (lifetime >= maxLifetime)
    {
        Destroy();
    }
}

CosmicEngine::Object *NetworkDemoProjectile::Clone() const
{
    return new NetworkDemoProjectile(ownerNetworkId, networkObjectId, damage, GetPosition(), GetSize(), maxLifetime);
}

int NetworkDemoProjectile::GetOwnerNetworkId() const { return ownerNetworkId; }
int NetworkDemoProjectile::GetDamage() const { return damage; }
int NetworkDemoProjectile::GetNetworkObjectId() const { return networkObjectId; }
float NetworkDemoProjectile::GetRemainingLifetime() const { return maxLifetime - lifetime; }
bool NetworkDemoProjectile::IsExpired() const { return lifetime >= maxLifetime; }

#endif // GAME_3D_CONFIGURATION
