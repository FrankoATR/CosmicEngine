#ifndef COSMIC_BODYMANAGER_HPP
#define COSMIC_BODYMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{

    class Body;
    class Object;

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        class GameGridCollisions;

    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

    #else
        #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif

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

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            GameGridCollisions *GridArea;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    public:
        static BodyManager &GetInstance();

        void init();
        void draw();
        void update();
        void Add(Body *body);
        void Remove(int entityId);
        void Clear();

        std::vector<Body *> FindAllByParent(Object* parent);
        
        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            void SetNewGridArea(GameGridCollisions*);

            void SetGridPosition(glm::vec2 newPosition);
            glm::vec2 GetGridPosition();

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION


        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // COSMIC_BODYMANAGER_HPP