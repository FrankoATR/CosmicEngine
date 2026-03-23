#ifndef COSMIC_BODYMANAGER_HPP
#define COSMIC_BODYMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{

    class Body;
    class Object;
    enum class CollisionType : int;
    class CollisionArea;
    class GameGridCollisions;
    class GameQuadTreeCollisions;

    class BodyManager
    {
    private:
        BodyManager();
        ~BodyManager();
        BodyManager(const BodyManager &) = delete;
        BodyManager &operator=(const BodyManager &) = delete;

        std::vector<Body *> bodys;
        std::vector<int> toDelete;
        int nextEntityId;
        CollisionArea *collisionArea;

    public:
        static BodyManager &GetInstance();

        void init();
        void draw();
        void update();
        void Add(Body *body);
        void Remove(int entityId);
        void Clear();

        std::vector<Body *> FindAllByParent(Object* parent);

        void SetCollisionArea(CollisionArea *newCollisionArea);
        void SetNewGridArea(GameGridCollisions *newGridArea);
        void SetNewQuadTreeArea(GameQuadTreeCollisions *newQuadTreeArea);
        CollisionType GetCollisionAreaType() const;
        bool HasCollisionArea() const;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            void CreateCollisionArea(CollisionType type, glm::vec2 position, glm::vec2 size, int subdivisionSize = 64, int maxDepth = 5, int maxObjectsPerNode = 4);
            void SetGridPosition(glm::vec2 newPosition);
            glm::vec2 GetGridPosition();

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            void CreateCollisionArea(CollisionType type, glm::vec3 position, glm::vec3 size, int subdivisionSize = 64, int maxDepth = 5, int maxObjectsPerNode = 4);
            void SetGridPosition(glm::vec3 newPosition);
            glm::vec3 GetGridPosition();

        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // COSMIC_BODYMANAGER_HPP