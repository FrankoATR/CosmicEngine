#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace WandEngine
{
    class GameLight;

    class LightManager
    {
    private:
        glm::vec3 ambientLight;
        std::vector<GameLight*> point_lights;
        std::vector<GameLight*> spot_lights;

        int nextEntityId;

        LightManager();
        ~LightManager();
        LightManager(const LightManager &) = delete;
        LightManager &operator=(const LightManager &) = delete;

    public:
        static LightManager &GetInstance()
        {
            static LightManager instance;
            return instance;
        }

        void Update(float deltaTime);
        void Draw();
        void Add(GameLight *actor);
        void Remove(int EntityId);
        GameLight *FindById(int EntityId);
        std::vector<GameLight *> FindByClassName(std::string className);
        std::vector<GameLight *> FindByPosition(glm::vec2 Position);
        std::vector<GameLight *> FindByMousePosition();
        std::vector<GameLight *> FindByLayer(int LayerId);
        std::vector<GameLight *> GetAll();
        void SortByLayer();
        void Clear();
    };

}

#endif // LIGHTMANAGER_HPP