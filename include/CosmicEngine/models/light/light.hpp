/**
 * @file light.hpp
 * @brief Declares the base light type used by the engine lighting system.
 */

#ifndef COSMIC_LIGHT_HPP
#define COSMIC_LIGHT_HPP

#include <glm/glm.hpp>
#include <string>

namespace CosmicEngine
{

    class LightManager;

    /**
     * @brief Represents a light source managed by the engine lighting system.
     *
     * Light stores position, attenuation parameters (constant, linear, quadratic),
     * and Phong shading terms (ambient, diffuse, specular, shininess).  Lights are
     * registered with the LightManager and their uniforms are propagated to shaders
     * each frame.
     *
     * @par Example — creating a point light (3D mode)
     * @code
     * auto* light = new CosmicEngine::Light(
     *     glm::vec3(0, 10, 0),    // position
     *     glm::vec3(0.2f),        // ambient
     *     glm::vec3(0.8f),        // diffuse
     *     glm::vec3(1.0f),        // specular
     *     1.0f, 0.09f, 0.032f,    // constant, linear, quadratic
     *     32.0f);                  // shininess
     * LightManager::GetInstance().Add(light);
     * @endcode
     */
    class Light
    {
    private:

        friend LightManager;

        int ID;
        bool visible;
        float constantLight;
        float linearLight;
        float quadraticLight;
        float shininess;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 position;
            glm::vec2 ambientLight;
            glm::vec2 diffuseLight;
            glm::vec2 specularLight;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 position;
            glm::vec3 ambientLight;
            glm::vec3 diffuseLight;
            glm::vec3 specularLight;

        #else
            #error "[Light] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    public:
        /** @brief Draws the light for debug or helper visualization. */
        void draw() const;

        /** @brief Performs light-specific initialization logic. */
        void init();

        /**
         * @brief Updates light-specific runtime logic.
         * @param deltaTime Fixed update time step.
         */
        void update(float deltaTime);

        /**
         * @brief Sets the unique identifier assigned by the light manager.
         * @param newID New light identifier.
         */
        void SetID(int newID);

        /**
         * @brief Returns the current light identifier.
         * @return Light identifier.
         */
        int GetID() const;

        /**
         * @brief Sets whether the light is active for rendering or lighting use.
         * @param visible New visibility state.
         */
        void SetVisible(bool visible);

        /**
         * @brief Returns whether the light is visible.
         * @return True when the light is marked as visible.
         */
        bool GetVisible() const;

        /** @brief Marks the light for removal from the runtime. */
        void Destroy();

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            /**
             * @brief Creates a 2D light.
             * @param position Light position.
             * @param ambientLight Ambient light contribution.
             * @param diffuseLight Diffuse light contribution.
             * @param specularLight Specular light contribution.
             * @param constantLight Constant attenuation factor.
             * @param linearLight Linear attenuation factor.
             * @param quadraticLight Quadratic attenuation factor.
             * @param shininess Specular shininess factor.
             * @param visible Initial visibility state.
             */
            Light(
                glm::vec2 position,
                glm::vec2 ambientLight = glm::vec2(1.0f),
                glm::vec2 diffuseLight = glm::vec2(1.0f),
                glm::vec2 specularLight = glm::vec2(1.0f),
                float constantLight = 1.0f,
                float linearLight = 0.09f,
                float quadraticLight = 0.032f,
                float shininess = 64.0f,
                bool visible = true);

            /** @brief Sets the 2D position of the light. */
            void SetPosition(glm::vec2 newPosition);
            /** @brief Returns the 2D position of the light. */
            glm::vec2 GetPosition() const;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            /**
             * @brief Creates a 3D light.
             * @param position Light position.
             * @param ambientLight Ambient light contribution.
             * @param diffuseLight Diffuse light contribution.
             * @param specularLight Specular light contribution.
             * @param constantLight Constant attenuation factor.
             * @param linearLight Linear attenuation factor.
             * @param quadraticLight Quadratic attenuation factor.
             * @param shininess Specular shininess factor.
             * @param visible Initial visibility state.
             */
            Light(
                glm::vec3 position,
                glm::vec3 ambientLight = glm::vec3(0.05f, 0.05f, 0.05f),
                glm::vec3 diffuseLight = glm::vec3(0.8f, 0.8f, 0.8f),
                glm::vec3 specularLight = glm::vec3(1.0f, 1.0f, 1.0f),
                float constantLight = 1.0f,
                float linearLight = 0.09f,
                float quadraticLight = 0.032f,
                float shininess = 64.0f,
                bool visible = true);

            /** @brief Sets the 3D position of the light. */
            void SetPosition(glm::vec3 newPosition);
            /** @brief Returns the 3D position of the light. */
            glm::vec3 GetPosition() const;

            /** @brief Sets the ambient light contribution. */
            void SetAmbientLight(glm::vec3 newAmbientLight);
            /** @brief Returns the ambient light contribution. */
            glm::vec3 GetAmbientLight() const;

            /** @brief Sets the diffuse light contribution. */
            void SetDiffuseLight(glm::vec3 newDiffuseLight);
            /** @brief Returns the diffuse light contribution. */
            glm::vec3 GetDiffuseLight() const;

            /** @brief Sets the specular light contribution. */
            void SetSpecularLight(glm::vec3 newSpecularLight);
            /** @brief Returns the specular light contribution. */
            glm::vec3 GetSpecularLight() const;

            /** @brief Sets the constant attenuation factor. */
            void SetConstantLight(float newConstantLight);
            /** @brief Returns the constant attenuation factor. */
            float GetConstantLight() const;

            /** @brief Sets the linear attenuation factor. */
            void SetLinearLight(float newLinearLight);
            /** @brief Returns the linear attenuation factor. */
            float GetLinearLight() const;

            /** @brief Sets the quadratic attenuation factor. */
            void SetQuadraticLight(float newQuadraticLight);
            /** @brief Returns the quadratic attenuation factor. */
            float GetQuadraticLight() const;

            /** @brief Sets the specular shininess factor. */
            void SetShininess(float newShininess);
            /** @brief Returns the specular shininess factor. */
            float GetShininess() const;

        #else
            #error "[Light] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // COSMIC_LIGHT_HPP