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
        * Point lights are split into two internal registries:
        * - static lights, which are cached and re-sorted only when the camera moves
        *   enough or the static registry changes,
        * - dynamic lights, which are evaluated every frame.
        *
        * When the number of visible point lights exceeds the active shader budget,
        * the manager keeps only the closest lights relative to the current camera.
        * The last active set receives a small hysteresis advantage so lights do not
        * flicker in and out every frame when several candidates have similar distance.
        *
        * This behavior is especially relevant for the lit 3D pipeline, where the
        * renderer uploads only a bounded number of point lights to the built-in
        * shaders each frame.
     *
        * @par Example - setting global ambient light
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

        std::vector<Light*> static_point_lights_resources;
        std::vector<Light*> dynamic_point_lights_resources;
        std::vector<Light*> cached_static_point_lights;
        std::vector<int> lastActivePointLightIds;
        std::vector<Shader*> target_shader_resources;
        std::vector<Light*> spot_lights;

        bool staticPointLightsDirty;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 lastStaticCacheCameraPosition;
        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 lastStaticCacheCameraPosition;
        #else
            #error "[LightManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

        int nextEntityId;
        int maxActivePointLights;

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
        /**
         * @brief Adds a light to the runtime according to its mobility and assigns it an identifier.
         *
         * Static point lights are routed to the cached static registry, while
         * movable lights are routed to the dynamic registry.
         */
        void Add(Light *light);
        /**
         * @brief Adds a point light to the static-light registry and assigns it an identifier.
         *
         * Static lights participate in the cached distance ordering reused across
         * frames until the cache is invalidated.
         */
        void AddStatic(Light *light);
        /**
         * @brief Adds a point light to the dynamic-light registry and assigns it an identifier.
         *
         * Dynamic lights are reconsidered every frame before selecting the active
         * point-light upload set.
         */
        void AddDynamic(Light *light);
        /** @brief Registers a shader that should receive lighting uniforms. */
        void RegisterShader(Shader *shader);
        /**
         * @brief Removes and deletes a light by identifier.
         * @param ID Value provided by the caller.
         */
        void Remove(int ID);
        /**
         * @brief Sets the maximum number of point lights uploaded to shaders each frame.
         * @param maxLights Requested light budget for the current runtime.
         *
         * The effective value is clamped by the shader-side GPU budget used by the
         * built-in lighting pipeline.
         */
        void SetMaxActivePointLights(int maxLights);
        /**
         * @brief Returns the current point-light upload budget.
         * @return The current point-light upload budget.
         */
        int GetMaxActivePointLights() const;

        /**
         * @brief Sets the global ambient light color.
         * @param newGlobalAmbientLightColor Value provided by the caller.
         */
        void SetGlobalAmbientLightColor(glm::vec3 newGlobalAmbientLightColor);
        /**
         * @brief Returns the global ambient light color.
         * @return The global ambient light color.
         */
        glm::vec3 GetGlobalAmbientLightColor() const;

        /**
         * @brief Sets the global ambient light direction.
         * @param newGlobalAmbientLightDirection Value provided by the caller.
         */
        void SetGlobalAmbientLightDirection(glm::vec3 newGlobalAmbientLightDirection);
        /**
         * @brief Returns the global ambient light direction.
         * @return The global ambient light direction.
         */
        glm::vec3 GetGlobalAmbientLightDirection() const;

        /**
         * @brief Sets the global ambient diffuse term.
         * @param newGlobalAmbientLightDiffuse Value provided by the caller.
         */
        void SetGlobalAmbientLightDiffuse(glm::vec3 newGlobalAmbientLightDiffuse);
        /**
         * @brief Returns the global ambient diffuse term.
         * @return The global ambient diffuse term.
         */
        glm::vec3 GetGlobalAmbientLightDiffuse() const;

        /**
         * @brief Sets the global ambient specular term.
         * @param newGlobalAmbientLightSpecular Value provided by the caller.
         */
        void SetGlobalAmbientLightSpecular(glm::vec3 newGlobalAmbientLightSpecular);
        /**
         * @brief Returns the global ambient specular term.
         * @return The global ambient specular term.
         */
        glm::vec3 GetGlobalAmbientLightSpecular() const;

        /** @brief Finds a light by identifier. */
        Light *FindById(int ID);
        /** @brief Finds lights overlapping a 2D position. */
        std::vector<Light *> FindByPosition(glm::vec2 Position);
        /** @brief Finds lights under the current mouse position. */
        std::vector<Light *> FindByMousePosition();
        /** @brief Returns a snapshot of all managed lights. */
        std::vector<Light *> GetAll();
        /**
         * @brief Marks the cached static-light ordering as stale.
         *
         * Call this after changing the position, visibility, or membership of a
         * static point light so the cached ordering can be rebuilt on the next update.
         */
        void InvalidateStaticPointLightCache();
        /** @brief Deletes and removes every managed light. */
        void Clear();
    };

}

#endif // COSMIC_LIGHTMANAGER_HPP
