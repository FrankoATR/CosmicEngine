#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace WandEngine
{
    class Light;
    class Shader;

    class LightManager
    {
    private:
        glm::vec3 globalAmbientLightColor;
        glm::vec3 globalAmbientLightDirection;
        glm::vec3 globalAmbientLightDiffuse;
        glm::vec3 globalAmbientLightSpecular;

        std::vector<Light*> point_lights_resources;
        std::vector<Shader*> target_shader_resources;
        std::vector<Light*> spot_lights;

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
        void Init();
        void Draw();
        void Add(Light *light);
        void RegisterShader(Shader *shader);
        void Remove(int ID);

        void SetGlobalAmbientLightColor(glm::vec3 newGlobalAmbientLightColor);
        glm::vec3 GetGlobalAmbientLightColor() const;

        void SetGlobalAmbientLightDirection(glm::vec3 newGlobalAmbientLightDirection);
        glm::vec3 GetGlobalAmbientLightDirection() const;

        void SetGlobalAmbientLightDiffuse(glm::vec3 newGlobalAmbientLightDiffuse);
        glm::vec3 GetGlobalAmbientLightDiffuse() const;

        void SetGlobalAmbientLightSpecular(glm::vec3 newGlobalAmbientLightSpecular);
        glm::vec3 GetGlobalAmbientLightSpecular() const;

        Light *FindById(int ID);
        std::vector<Light *> FindByPosition(glm::vec2 Position);
        std::vector<Light *> FindByMousePosition();
        std::vector<Light *> GetAll();
        void Clear();
    };

}

#endif // LIGHTMANAGER_HPP