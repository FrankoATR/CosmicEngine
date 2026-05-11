#include "LightManager.hpp"
#include "../../Models/Light/Light.hpp"
#include "../../Models/Shader/Shader.hpp"
#include "../../Utils/Log.hpp"

namespace WandEngine
{

    LightManager::LightManager()
    {
        RUNTIME_INFO("Light manager created");
    }

    LightManager::~LightManager()
    {
        RUNTIME_INFO("Light manager destroyed");
    }

    void LightManager::Init()
    {
        this->globalAmbientLightDirection = glm::vec3(0.05f, 0.05f, 0.05f);
        this->globalAmbientLightColor = glm::vec3(0.05f, 0.05f, 0.05f);
        this->globalAmbientLightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        this->globalAmbientLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
        RUNTIME_INFO("Light manager initialized");
    }

    void LightManager::Update(float deltaTime)
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

    void LightManager::Draw()
    {

    }

    void LightManager::Add(Light *light)
    {
        light->ID = nextEntityId++;
        // light->Init();
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
    }

    std::vector<Light *> LightManager::FindByPosition(glm::vec2 Position)
    {
    }

    std::vector<Light *> LightManager::FindByMousePosition()
    {
    }

    std::vector<Light *> LightManager::GetAll()
    {
        return this->point_lights_resources;
    }

    void LightManager::Clear()
    {
    }

}