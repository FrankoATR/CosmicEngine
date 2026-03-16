/**
 * @file light.hpp
 * @brief Contains Code for Lights
 *
 * This file include all 2D and 3D configurations for Lights
 *
 * @author Francisco Rosa
 * @date 2025-04-21
 * @version 1.0
 * @copyright MIT License
 */

#ifndef COSMIC_LIGHT_HPP
#define COSMIC_LIGHT_HPP

#include <glm/glm.hpp>
#include <string>

namespace CosmicEngine
{

    class LightManager;

    /**
     * @class Light
     * @brief Class with the methods needed to implement entities.
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
        /**
         * @brief draw the Light on window.
         */
        void draw() const;

        /**
         * @brief Initial configurations for the Light.
         */
        void init();

        /**
         * @brief Logic for the Light.
         * @param deltaTime Value to maintain consistency in movement calculations within the logic.
         */
        void update(float deltaTime);

        /**
         * @brief Set the unique ID for the Light.
         * @param newID Changes the current ID of the Light to this one.
         */
        void SetID(int newID);

        /**
         * @brief Gets the current ID value of the Light.
         * @return Current ID of the Light.
         */
        int GetID() const;

        /**
         * @brief Sets the visibility value of the Light.
         * @param visible Must be true or false.
         * @example obj->SetVisible(true);
         */
        void SetVisible(bool visible);

        /**
         * @brief Gets whether the Light is currently visible.
         * @return Current visible value of the Light.
         */
        bool GetVisible() const;

        void Destroy();

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
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

            void SetPosition(glm::vec2 newPosition);
            glm::vec2 GetPosition() const;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
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

            void SetPosition(glm::vec3 newPosition);
            glm::vec3 GetPosition() const;

            void SetAmbientLight(glm::vec3 newAmbientLight);
            glm::vec3 GetAmbientLight() const;

            void SetDiffuseLight(glm::vec3 newDiffuseLight);
            glm::vec3 GetDiffuseLight() const;

            void SetSpecularLight(glm::vec3 newSpecularLight);
            glm::vec3 GetSpecularLight() const;

            void SetConstantLight(float newConstantLight);
            float GetConstantLight() const;

            void SetLinearLight(float newLinearLight);
            float GetLinearLight() const;

            void SetQuadraticLight(float newQuadraticLight);
            float GetQuadraticLight() const;

            void SetShininess(float newShininess);
            float GetShininess() const;

        #else
            #error "[Light] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // COSMIC_LIGHT_HPP