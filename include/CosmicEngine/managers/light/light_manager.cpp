/**
 * @file light_manager.cpp
 * @brief Implements the runtime light manager used by the engine renderer.
 */

#include "light_manager.hpp"
#include "../../models/light/light.hpp"
#include "../../models/shader/shader.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{

    LightManager::LightManager()
    {
		RUNTIME_LIFECYCLE("Light manager", "created");
    }

    LightManager::~LightManager()
    {
		RUNTIME_LIFECYCLE("Light manager", "destroyed");
    }
    
    void LightManager::init()
    {
        this->globalAmbientLightDirection = glm::vec3(0.05f, 0.05f, 0.05f);
        this->globalAmbientLightColor = glm::vec3(0.05f, 0.05f, 0.05f);
        this->globalAmbientLightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        this->globalAmbientLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
        RUNTIME_LIFECYCLE("Light manager", "initialized");
    }

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

    void LightManager::update(float deltaTime)
    {
        for (auto shader : target_shader_resources)
        {
            for (size_t i = 0; i < point_lights_resources.size(); ++i)   // OPTIMIZE, BECAUSE THE CURRENT MAX IS 4
            {
                const auto &light = point_lights_resources[i];

                shader->Use();

                shader->SetVec3("dirLight.direction", globalAmbientLightDirection);
                shader->SetVec3("dirLight.ambient", globalAmbientLightColor);
                shader->SetVec3("dirLight.diffuse", globalAmbientLightDiffuse);
                shader->SetVec3("dirLight.specular", globalAmbientLightSpecular);
                shader->SetFloat("material.shininess", light->shininess);

                shader->SetVec2("pointLights["+ std::to_string(i) +"].position", light->position);
                shader->SetVec2("pointLights["+ std::to_string(i) +"].ambient", light->ambientLight);
                shader->SetVec2("pointLights["+ std::to_string(i) +"].diffuse", light->diffuseLight);
                shader->SetVec2("pointLights["+ std::to_string(i) +"].specular", light->specularLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].constant", light->constantLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].linear", light->linearLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].quadratic", light->quadraticLight);


                shader->EndUse();
            }
        }
    }
    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

    void LightManager::update(float deltaTime)
    {
        for (auto shader : target_shader_resources)
        {
            for (size_t i = 0; i < point_lights_resources.size(); ++i)   // OPTIMIZE, BECAUSE THE CURRENT MAX IS 4
            {
                const auto &light = point_lights_resources[i];

                shader->Use();

                shader->SetVec3("dirLight.direction", globalAmbientLightDirection);
                shader->SetVec3("dirLight.ambient", globalAmbientLightColor);
                shader->SetVec3("dirLight.diffuse", globalAmbientLightDiffuse);
                shader->SetVec3("dirLight.specular", globalAmbientLightSpecular);
                shader->SetFloat("material.shininess", light->shininess);

                shader->SetVec3("pointLights["+ std::to_string(i) +"].position", light->position);
                shader->SetVec3("pointLights["+ std::to_string(i) +"].ambient", light->ambientLight);
                shader->SetVec3("pointLights["+ std::to_string(i) +"].diffuse", light->diffuseLight);
                shader->SetVec3("pointLights["+ std::to_string(i) +"].specular", light->specularLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].constant", light->constantLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].linear", light->linearLight);
                shader->SetFloat("pointLights["+ std::to_string(i) +"].quadratic", light->quadraticLight);


                shader->EndUse();
            }
        }
    }
    #else
        #error "[LightManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif



    void LightManager::draw()
    {

    }

    void LightManager::Add(Light *light)
    {
        light->ID = nextEntityId++;
        // light->init();
        point_lights_resources.push_back(light); 
    }

    void LightManager::RegisterShader(Shader *shader)
    {
        target_shader_resources.push_back(shader);
    }

    void LightManager::Remove(int ID)
    {
    }

    void LightManager::SetGlobalAmbientLightColor(glm::vec3 newGlobalAmbientLightColor)
    {
        this->globalAmbientLightColor = newGlobalAmbientLightColor;
    }

    glm::vec3 LightManager::GetGlobalAmbientLightColor() const
    {
        return this->globalAmbientLightColor;
    }

    void LightManager::SetGlobalAmbientLightDirection(glm::vec3 newGlobalAmbientLightDirection)
    {
        this->globalAmbientLightDirection = newGlobalAmbientLightDirection;
    }

    glm::vec3 LightManager::GetGlobalAmbientLightDirection() const
    {
        return this->globalAmbientLightDirection;
    }

    void LightManager::SetGlobalAmbientLightDiffuse(glm::vec3 newGlobalAmbientLightDiffuse)
    {
        this->globalAmbientLightDiffuse = newGlobalAmbientLightDiffuse;
    }

    glm::vec3 LightManager::GetGlobalAmbientLightDiffuse() const
    {
        return this->globalAmbientLightDiffuse;
    }

    void LightManager::SetGlobalAmbientLightSpecular(glm::vec3 newGlobalAmbientLightSpecular)
    {
        this->globalAmbientLightSpecular = newGlobalAmbientLightSpecular;
    }

    glm::vec3 LightManager::GetGlobalAmbientLightSpecular() const
    {
        return this->globalAmbientLightSpecular;
    }

    Light *LightManager::FindById(int ID)
    {
        auto it = std::find_if(point_lights_resources.begin(), point_lights_resources.end(), [ID](Light *light)
        {
            return light && light->GetID() == ID;
        });

        if (it != point_lights_resources.end())
        {
            return *it;
        }

        return nullptr;
    }

    std::vector<Light *> LightManager::FindByPosition(glm::vec2 Position)
    {
        std::vector<Light *> matches;
        (void)Position;
        return matches;
    }

    std::vector<Light *> LightManager::FindByMousePosition()
    {
        return {};
    }

    std::vector<Light *> LightManager::GetAll()
    {
        return this->point_lights_resources;
    }

    void LightManager::Clear()
    {
        for (auto *light : point_lights_resources)
        {
            delete light;
        }
        point_lights_resources.clear();
        spot_lights.clear();
        target_shader_resources.clear();
        nextEntityId = 0;
		RUNTIME_LIFECYCLE("Light manager", "cleared");
    }

}