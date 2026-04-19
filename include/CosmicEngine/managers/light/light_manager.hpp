#ifndef COSMIC_LIGHTMANAGER_HPP
#define COSMIC_LIGHTMANAGER_HPP

/**
 * @file light_manager.hpp
 * @brief Declares the light manager used by the engine renderer.
 */

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace CosmicEngine
{
    class Light;
    class Shader;

    /**
     * @brief Manages runtime lights and propagates lighting state to registered shaders.
     *
     * LightManager owns all active lights and pushes their uniforms into every
     * registered shader each frame.  It also exposes a global ambient light that
     * applies to the entire scene independently of point/spot lights.
     *
     * @par Example — setting global ambient light
     * @code
     * auto& lm = CosmicEngine::LightManager::GetInstance();
     * lm.SetGlobalAmbientLightColor(glm::vec3(0.15f));
     * lm.SetGlobalAmbientLightDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
     * @endcode
     */
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
        /** @brief Returns the singleton instance of the light manager. */
        static LightManager &GetInstance()
        {
            static LightManager instance;
            return instance;
        }

        /**
         * @brief Updates light runtime state.
         * @param deltaTime Fixed update time step.
         */
        void update(float deltaTime);
        /** @brief Initializes the light manager state. */
        void init();
        /** @brief Draws managed lights for debug or helper visualization. */
        void draw();
        /** @brief Adds a light to the runtime and assigns it an identifier. */
        void Add(Light *light);
        /** @brief Registers a shader that should receive lighting uniforms. */
        void RegisterShader(Shader *shader);
        /** @brief Removes and deletes a light by identifier. */
        void Remove(int ID);

        /** @brief Sets the global ambient light color. */
        void SetGlobalAmbientLightColor(glm::vec3 newGlobalAmbientLightColor);
        /** @brief Returns the global ambient light color. */
        glm::vec3 GetGlobalAmbientLightColor() const;

        /** @brief Sets the global ambient light direction. */
        void SetGlobalAmbientLightDirection(glm::vec3 newGlobalAmbientLightDirection);
        /** @brief Returns the global ambient light direction. */
        glm::vec3 GetGlobalAmbientLightDirection() const;

        /** @brief Sets the global ambient diffuse term. */
        void SetGlobalAmbientLightDiffuse(glm::vec3 newGlobalAmbientLightDiffuse);
        /** @brief Returns the global ambient diffuse term. */
        glm::vec3 GetGlobalAmbientLightDiffuse() const;

        /** @brief Sets the global ambient specular term. */
        void SetGlobalAmbientLightSpecular(glm::vec3 newGlobalAmbientLightSpecular);
        /** @brief Returns the global ambient specular term. */
        glm::vec3 GetGlobalAmbientLightSpecular() const;

        /** @brief Finds a light by identifier. */
        Light *FindById(int ID);
        /** @brief Finds lights overlapping a 2D position. */
        std::vector<Light *> FindByPosition(glm::vec2 Position);
        /** @brief Finds lights under the current mouse position. */
        std::vector<Light *> FindByMousePosition();
        /** @brief Returns a snapshot of all managed lights. */
        std::vector<Light *> GetAll();
        /** @brief Deletes and removes every managed light. */
        void Clear();
    };

}

#endif // COSMIC_LIGHTMANAGER_HPP