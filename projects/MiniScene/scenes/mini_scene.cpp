#include "mini_scene.hpp"

#include "../entities/player_object.hpp"
#include "../entities/obstacle_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include AUDIOMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include BODYMANAGER_HEADER
#include <CosmicEngine/collisions/collision_area.hpp>

namespace MiniScene
{
    void MiniScene::loadResources()
    {
        RS_MN.LoadTexture("player", "assets/textures/player.png");
        RS_MN.LoadTexture("obstacle", "assets/textures/obstacle.png");
        AUD_MN.Load("hit", "assets/audio/hit.wav", CosmicEngine::SoundType::SFX);
    }

    void MiniScene::init()
    {
        // World camera is centered on the origin: visible area [-400,400] x [-300,300].
        BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid,
                                   glm::vec2(-400.0f, -300.0f), glm::vec2(800.0f, 600.0f));
        OBJ_MN.Add(new PlayerObject(glm::vec2(-250.0f, -32.0f)));
        OBJ_MN.Add(new ObstacleObject(glm::vec2(150.0f, -32.0f)));
    }

    void MiniScene::update(double) {}
}
