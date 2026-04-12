#ifndef NETWORK_DEMO_PROJECTILE_HPP
#define NETWORK_DEMO_PROJECTILE_HPP

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/body/body.hpp>
#include <glm/glm.hpp>
#include <string>

/**
 * @brief A projectile/hazard object in the network demo.
 * Spawned by players, lasts a set duration (5 seconds), and deals random damage (1-3)
 * to any player that touches it.
 */
class NetworkDemoProjectile : public CosmicEngine::Object
{
private:
    int ownerNetworkId;
    int damage;
    float lifetime;
    float maxLifetime;
    int bodyId;
    int networkObjectId;

    void CreateBody();

public:
    static const std::string &StaticClassName();

    NetworkDemoProjectile(
        int ownerNetworkId,
        int networkObjectId,
        int damage,
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 size = glm::vec3(1.0f),
        float maxLifetime = 5.0f);

    ~NetworkDemoProjectile() override;

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;

    int GetOwnerNetworkId() const;
    int GetDamage() const;
    int GetNetworkObjectId() const;
    float GetRemainingLifetime() const;
    bool IsExpired() const;
};

#endif // GAME_3D_CONFIGURATION

#endif // NETWORK_DEMO_PROJECTILE_HPP
