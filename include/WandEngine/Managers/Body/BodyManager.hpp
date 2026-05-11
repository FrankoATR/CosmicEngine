#ifndef BODYMANAGER_HPP
#define BODYMANAGER_HPP

#include "../../Utils/Configurations.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace WandEngine
{

    class Body;
    class Object;

    #if defined(GAME_2D_CONFIGURATION)
        class GameGridCollisions;

    #elif defined(GAME_3D_CONFIGURATION)

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

        #if defined(GAME_2D_CONFIGURATION)
            GameGridCollisions *GridArea;

        #elif defined(GAME_3D_CONFIGURATION)

        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    public:
        static BodyManager &GetInstance();

        void Init();
        void Draw();
        void Update();
        void Add(Body *body);
        void Remove(int entityId);
        void Clear();

        std::vector<Body *> FindAllByParent(Object* parent);
        
        #if defined(GAME_2D_CONFIGURATION)
            void SetNewGridArea(GameGridCollisions*);

            void SetGridPosition(glm::vec2 newPosition);
            glm::vec2 GetGridPosition();

        #elif defined(GAME_3D_CONFIGURATION)


        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // BODYMANAGER_HPP