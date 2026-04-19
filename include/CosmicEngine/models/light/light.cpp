/**
 * @file light.cpp
 * @brief Implements the base light type used by the engine lighting system.
 */

#include "light.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{
    Light::~Light()
    {
        RUNTIME_LIFECYCLE("Light", "destroyed");
    }

    void Light::draw() const
    {

    }

    void Light::init()
    {
		RUNTIME_LIFECYCLE("Light", "initialized");
    }

    void Light::update(float deltaTime)
    {

    }

    void Light::SetID(int newID)
    {
        this->ID = newID;
    }

    int Light::GetID() const
    {
        return this->ID;
    }

    void Light::SetVisible(bool visible)
    {
        this->visible = visible;
    }

    bool Light::GetVisible() const
    {
        return this->visible;
    }

    void Light::Destroy()
    {
        this->visible = false;
		RUNTIME_LIFECYCLE("Light", "destroyed");
    }


    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        Light::Light(
            glm::vec2 position,
            glm::vec2 ambientLight,
            glm::vec2 diffuseLight,
            glm::vec2 specularLight,
            float constantLight,
            float linearLight,
            float quadraticLight,
            float shininess,
            bool visible
        )
        {
            this->position = position;
            this->ambientLight = ambientLight;
            this->diffuseLight = diffuseLight;
            this->specularLight = specularLight;
            this->constantLight = constantLight;
            this->linearLight = linearLight;
            this->quadraticLight = quadraticLight;
            this->shininess = shininess;
            this->visible = visible;
			RUNTIME_LIFECYCLE("Light", "created");
        }

        void Light::SetPosition(glm::vec2 newPosition)
        {
            this->position = newPosition;
        }

        glm::vec2 Light::GetPosition() const
        {
            return this->position;
        }


    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        Light::Light(
            glm::vec3 position,
            glm::vec3 ambientLight,
            glm::vec3 diffuseLight,
            glm::vec3 specularLight,
            float constantLight,
            float linearLight,
            float quadraticLight,
            float shininess,
            bool visible
        )
        {
            this->position = position;
            this->ambientLight = ambientLight;
            this->diffuseLight = diffuseLight;
            this->specularLight = specularLight;
            this->constantLight = constantLight;
            this->linearLight = linearLight;
            this->quadraticLight = quadraticLight;
            this->shininess = shininess;
            this->visible = visible;
			RUNTIME_LIFECYCLE("Light", "created");
        }
        
        void Light::SetPosition(glm::vec3 newPosition)
        {
            this->position = newPosition;
        }

        glm::vec3 Light::GetPosition() const
        {
            return this->position;
        }

        void Light::SetAmbientLight(glm::vec3 newAmbientLight)
        {
            this->ambientLight = newAmbientLight;
        }

        glm::vec3 Light::GetAmbientLight() const
        {
            return this->ambientLight;
        }

        void Light::SetDiffuseLight(glm::vec3 newDiffuseLight)
        {
            this->diffuseLight = newDiffuseLight;
        }

        glm::vec3 Light::GetDiffuseLight() const
        {
            return this->diffuseLight;
        }

        void Light::SetSpecularLight(glm::vec3 newSpecularLight)
        {
            this->specularLight = newSpecularLight;
        }

        glm::vec3 Light::GetSpecularLight() const
        {
            return this->specularLight;
        }

        void Light::SetConstantLight(float newConstantLight)
        {
            this->constantLight = newConstantLight;
        }

        float Light::GetConstantLight() const
        {
            return this->constantLight;
        }

        void Light::SetLinearLight(float newLinearLight)
        {
            this->linearLight = newLinearLight;
        }

        float Light::GetLinearLight() const
        {
            return this->linearLight;
        }

        void Light::SetQuadraticLight(float newQuadraticLight)
        {
            this->quadraticLight = newQuadraticLight;
        }

        float Light::GetQuadraticLight() const
        {
            return this->quadraticLight;
        }

        void Light::SetShininess(float newShininess)
        {
            this->shininess = newShininess;
        }

        float Light::GetShininess() const
        {
            return this->shininess;
        }
    
    #else
        #error "[Light] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif

}