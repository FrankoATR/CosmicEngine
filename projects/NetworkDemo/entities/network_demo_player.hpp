#ifndef NETWORK_DEMO_PLAYER_HPP
#define NETWORK_DEMO_PLAYER_HPP

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/body/body.hpp>
#include <glm/glm.hpp>
#include <string>

/**
 * @brief A player object in the network demo.
 * Each player has a position, health, and a color to distinguish them.
 * The local player is controlled via keyboard input.
 * Remote players are updated via network state messages.
 */
class NetworkDemoPlayer : public CosmicEngine::Object
{
private:
    static glm::vec3 movementAreaPosition;
    static glm::vec3 movementAreaSize;

    int networkId;
    std::string playerName;
    int health;
    int maxHealth;
    float moveSpeed;
    int bodyId;
    bool isLocal;
    bool grounded;

    void CreateBody();
    void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);
    void ClampToMovementArea();

public:
    static const std::string &StaticClassName();
    static void SetMovementArea(glm::vec3 position, glm::vec3 size);

    NetworkDemoPlayer(
        int networkId,
        const std::string &playerName,
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 size = glm::vec3(2.0f),
        bool isLocal = false);

    ~NetworkDemoPlayer() override;

    void draw() const override;
    void update(float deltaTime) override;
    CosmicEngine::Object *Clone() const override;

    int GetNetworkId() const;
    const std::string &GetPlayerName() const;

    void SetHealth(int health);
    int GetHealth() const;
    int GetMaxHealth() const;
    bool IsAlivePlayer() const;

    void TakeDamage(int amount);
    void SetIsLocal(bool local);
    bool GetIsLocal() const;
    bool IsGrounded() const;

    float GetMoveSpeed() const;
};

#endif // GAME_3D_CONFIGURATION

#endif // NETWORK_DEMO_PLAYER_HPP
