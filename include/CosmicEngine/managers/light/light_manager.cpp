/**
 * @file light_manager.cpp
 * @brief Implements the runtime light manager used by the engine renderer.
 */

#include "light_manager.hpp"
#include "../camera/camera_manager.hpp"
#include "../../models/light/light.hpp"
#include "../../models/shader/shader.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{
    namespace
    {
        constexpr size_t kMaxGpuPointLights = 8;
        constexpr float kDefaultMaterialShininess = 32.0f;
        constexpr float kStaticPointLightRefreshDistanceSq = 0.04f;
        constexpr float kActiveLightHysteresisDistance = 10.0f;
    }

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
        this->maxActivePointLights = static_cast<int>(kMaxGpuPointLights);
        this->staticPointLightsDirty = true;
        this->nextEntityId = 0;
        this->cached_static_point_lights.clear();
        this->static_point_lights_resources.clear();
        this->dynamic_point_lights_resources.clear();
        this->lastActivePointLightIds.clear();
        RUNTIME_LIFECYCLE("Light manager", "initialized");
    }

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

    void LightManager::update(float deltaTime)
    {
        (void)deltaTime;

        std::vector<Light *> activeDynamicPointLights;
        activeDynamicPointLights.reserve(dynamic_point_lights_resources.size());

        const auto computeLightPriority = [this](Light *light, const glm::vec2 &cameraPosition)
        {
            const glm::vec2 delta = light->position - cameraPosition;
            const float distance = glm::length(delta);
            const bool wasActiveLastFrame = std::find(lastActivePointLightIds.begin(), lastActivePointLightIds.end(), light->GetID()) != lastActivePointLightIds.end();
            return std::max(0.0f, distance - (wasActiveLastFrame ? kActiveLightHysteresisDistance : 0.0f));
        };

        for (Light *light : dynamic_point_lights_resources)
        {
            if (!light || !light->GetVisible())
            {
                continue;
            }

            activeDynamicPointLights.push_back(light);
        }

        const glm::vec2 cameraPosition = CameraManager::GetInstance().GetPosition();
        std::stable_sort(activeDynamicPointLights.begin(), activeDynamicPointLights.end(), [&cameraPosition, &computeLightPriority](Light *left, Light *right)
        {
            if (!left)
            {
                return false;
            }
            if (!right)
            {
                return true;
            }

            const float leftPriority = computeLightPriority(left, cameraPosition);
            const float rightPriority = computeLightPriority(right, cameraPosition);
            if (leftPriority == rightPriority)
            {
                return left->GetID() < right->GetID();
            }

            return leftPriority < rightPriority;
        });

        const glm::vec2 staticCameraDelta = cameraPosition - lastStaticCacheCameraPosition;
        if (staticPointLightsDirty || glm::dot(staticCameraDelta, staticCameraDelta) >= kStaticPointLightRefreshDistanceSq)
        {
            cached_static_point_lights = static_point_lights_resources;
            std::stable_sort(cached_static_point_lights.begin(), cached_static_point_lights.end(), [&cameraPosition, &computeLightPriority](Light *left, Light *right)
            {
                if (!left)
                {
                    return false;
                }
                if (!right)
                {
                    return true;
                }

                const float leftPriority = computeLightPriority(left, cameraPosition);
                const float rightPriority = computeLightPriority(right, cameraPosition);
                if (leftPriority == rightPriority)
                {
                    return left->GetID() < right->GetID();
                }

                return leftPriority < rightPriority;
            });

            staticPointLightsDirty = false;
            lastStaticCacheCameraPosition = cameraPosition;
        }

        std::vector<Light *> activePointLights;
        activePointLights.reserve(static_cast<size_t>(maxActivePointLights));

        size_t staticIndex = 0;
        size_t dynamicIndex = 0;
        while (activePointLights.size() < static_cast<size_t>(maxActivePointLights))
        {
            while (staticIndex < cached_static_point_lights.size() && (!cached_static_point_lights[staticIndex] || !cached_static_point_lights[staticIndex]->GetVisible()))
            {
                ++staticIndex;
            }

            const bool hasStatic = staticIndex < cached_static_point_lights.size();
            const bool hasDynamic = dynamicIndex < activeDynamicPointLights.size();
            if (!hasStatic && !hasDynamic)
            {
                break;
            }

            if (!hasStatic)
            {
                activePointLights.push_back(activeDynamicPointLights[dynamicIndex++]);
                continue;
            }

            if (!hasDynamic)
            {
                activePointLights.push_back(cached_static_point_lights[staticIndex++]);
                continue;
            }

            const float staticPriority = computeLightPriority(cached_static_point_lights[staticIndex], cameraPosition);
            const float dynamicPriority = computeLightPriority(activeDynamicPointLights[dynamicIndex], cameraPosition);

            if (staticPriority <= dynamicPriority)
            {
                activePointLights.push_back(cached_static_point_lights[staticIndex++]);
            }
            else
            {
                activePointLights.push_back(activeDynamicPointLights[dynamicIndex++]);
            }
        }

        const float materialShininess = activePointLights.empty()
            ? kDefaultMaterialShininess
            : activePointLights.back()->shininess;

        lastActivePointLightIds.clear();
        lastActivePointLightIds.reserve(activePointLights.size());
        for (Light *light : activePointLights)
        {
            if (light)
            {
                lastActivePointLightIds.push_back(light->GetID());
            }
        }

        for (auto shader : target_shader_resources)
        {
            if (!shader)
            {
                continue;
            }

            shader->Use();
            shader->SetVec3("dirLight.direction", globalAmbientLightDirection);
            shader->SetVec3("dirLight.ambient", globalAmbientLightColor);
            shader->SetVec3("dirLight.diffuse", globalAmbientLightDiffuse);
            shader->SetVec3("dirLight.specular", globalAmbientLightSpecular);
            shader->SetFloat("material.shininess", materialShininess);
            shader->SetInt("activePointLightCount", static_cast<int>(activePointLights.size()));

            for (size_t i = 0; i < activePointLights.size(); ++i)
            {
                const auto *light = activePointLights[i];

                shader->SetVec2("pointLights[" + std::to_string(i) + "].position", light->position);
                shader->SetVec2("pointLights[" + std::to_string(i) + "].ambient", light->ambientLight);
                shader->SetVec2("pointLights[" + std::to_string(i) + "].diffuse", light->diffuseLight);
                shader->SetVec2("pointLights[" + std::to_string(i) + "].specular", light->specularLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].constant", light->constantLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].linear", light->linearLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].quadratic", light->quadraticLight);
            }

            shader->EndUse();
        }
    }
    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

    void LightManager::update(float deltaTime)
    {
        (void)deltaTime;

        std::vector<Light *> activeDynamicPointLights;
        activeDynamicPointLights.reserve(dynamic_point_lights_resources.size());

        const auto computeLightPriority = [this](Light *light, const glm::vec3 &cameraPosition)
        {
            const glm::vec3 delta = light->position - cameraPosition;
            const float distance = glm::length(delta);
            const bool wasActiveLastFrame = std::find(lastActivePointLightIds.begin(), lastActivePointLightIds.end(), light->GetID()) != lastActivePointLightIds.end();
            return std::max(0.0f, distance - (wasActiveLastFrame ? kActiveLightHysteresisDistance : 0.0f));
        };

        for (Light *light : dynamic_point_lights_resources)
        {
            if (!light || !light->GetVisible())
            {
                continue;
            }

            activeDynamicPointLights.push_back(light);
        }

        const glm::vec3 cameraPosition = CameraManager::GetInstance().GetPosition();
        std::stable_sort(activeDynamicPointLights.begin(), activeDynamicPointLights.end(), [&cameraPosition, &computeLightPriority](Light *left, Light *right)
        {
            if (!left)
            {
                return false;
            }
            if (!right)
            {
                return true;
            }

            const float leftPriority = computeLightPriority(left, cameraPosition);
            const float rightPriority = computeLightPriority(right, cameraPosition);
            if (leftPriority == rightPriority)
            {
                return left->GetID() < right->GetID();
            }

            return leftPriority < rightPriority;
        });

        const glm::vec3 staticCameraDelta = cameraPosition - lastStaticCacheCameraPosition;
        if (staticPointLightsDirty || glm::dot(staticCameraDelta, staticCameraDelta) >= kStaticPointLightRefreshDistanceSq)
        {
            cached_static_point_lights = static_point_lights_resources;
            std::stable_sort(cached_static_point_lights.begin(), cached_static_point_lights.end(), [&cameraPosition, &computeLightPriority](Light *left, Light *right)
            {
                if (!left)
                {
                    return false;
                }
                if (!right)
                {
                    return true;
                }

                const float leftPriority = computeLightPriority(left, cameraPosition);
                const float rightPriority = computeLightPriority(right, cameraPosition);
                if (leftPriority == rightPriority)
                {
                    return left->GetID() < right->GetID();
                }

                return leftPriority < rightPriority;
            });

            staticPointLightsDirty = false;
            lastStaticCacheCameraPosition = cameraPosition;
        }

        std::vector<Light *> activePointLights;
        activePointLights.reserve(static_cast<size_t>(maxActivePointLights));

        size_t staticIndex = 0;
        size_t dynamicIndex = 0;
        while (activePointLights.size() < static_cast<size_t>(maxActivePointLights))
        {
            while (staticIndex < cached_static_point_lights.size() && (!cached_static_point_lights[staticIndex] || !cached_static_point_lights[staticIndex]->GetVisible()))
            {
                ++staticIndex;
            }

            const bool hasStatic = staticIndex < cached_static_point_lights.size();
            const bool hasDynamic = dynamicIndex < activeDynamicPointLights.size();
            if (!hasStatic && !hasDynamic)
            {
                break;
            }

            if (!hasStatic)
            {
                activePointLights.push_back(activeDynamicPointLights[dynamicIndex++]);
                continue;
            }

            if (!hasDynamic)
            {
                activePointLights.push_back(cached_static_point_lights[staticIndex++]);
                continue;
            }

            const float staticPriority = computeLightPriority(cached_static_point_lights[staticIndex], cameraPosition);
            const float dynamicPriority = computeLightPriority(activeDynamicPointLights[dynamicIndex], cameraPosition);

            if (staticPriority <= dynamicPriority)
            {
                activePointLights.push_back(cached_static_point_lights[staticIndex++]);
            }
            else
            {
                activePointLights.push_back(activeDynamicPointLights[dynamicIndex++]);
            }
        }

        const float materialShininess = activePointLights.empty()
            ? kDefaultMaterialShininess
            : activePointLights.back()->shininess;

        lastActivePointLightIds.clear();
        lastActivePointLightIds.reserve(activePointLights.size());
        for (Light *light : activePointLights)
        {
            if (light)
            {
                lastActivePointLightIds.push_back(light->GetID());
            }
        }

        for (auto shader : target_shader_resources)
        {
            if (!shader)
            {
                continue;
            }

            shader->Use();
            shader->SetVec3("dirLight.direction", globalAmbientLightDirection);
            shader->SetVec3("dirLight.ambient", globalAmbientLightColor);
            shader->SetVec3("dirLight.diffuse", globalAmbientLightDiffuse);
            shader->SetVec3("dirLight.specular", globalAmbientLightSpecular);
            shader->SetFloat("material.shininess", materialShininess);
            shader->SetInt("activePointLightCount", static_cast<int>(activePointLights.size()));

            for (size_t i = 0; i < activePointLights.size(); ++i)
            {
                const auto *light = activePointLights[i];

                shader->SetVec3("pointLights[" + std::to_string(i) + "].position", light->position);
                shader->SetVec3("pointLights[" + std::to_string(i) + "].ambient", light->ambientLight);
                shader->SetVec3("pointLights[" + std::to_string(i) + "].diffuse", light->diffuseLight);
                shader->SetVec3("pointLights[" + std::to_string(i) + "].specular", light->specularLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].constant", light->constantLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].linear", light->linearLight);
                shader->SetFloat("pointLights[" + std::to_string(i) + "].quadratic", light->quadraticLight);
            }

            shader->EndUse();
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
        if (!light)
        {
            return;
        }

        if (light->GetMobility() == LightMobility::Static)
        {
            AddStatic(light);
            return;
        }

        AddDynamic(light);
    }

    void LightManager::AddStatic(Light *light)
    {
        if (!light)
        {
            return;
        }

        light->ID = nextEntityId++;
        static_point_lights_resources.push_back(light);
        InvalidateStaticPointLightCache();
    }

    void LightManager::AddDynamic(Light *light)
    {
        if (!light)
        {
            return;
        }

        light->ID = nextEntityId++;
        dynamic_point_lights_resources.push_back(light);
    }

    void LightManager::RegisterShader(Shader *shader)
    {
        if (!shader)
        {
            return;
        }

        auto it = std::find(target_shader_resources.begin(), target_shader_resources.end(), shader);
        if (it != target_shader_resources.end())
        {
            return;
        }

        target_shader_resources.push_back(shader);
    }

    void LightManager::Remove(int ID)
    {
        auto removeById = [ID](std::vector<Light *> &lights)
        {
            auto it = std::find_if(lights.begin(), lights.end(), [ID](Light *light)
            {
                return light && light->GetID() == ID;
            });

            if (it == lights.end())
            {
                return false;
            }

            delete *it;
            lights.erase(it);
            return true;
        };

        if (removeById(static_point_lights_resources))
        {
            InvalidateStaticPointLightCache();
            return;
        }

        removeById(dynamic_point_lights_resources);
    }

    void LightManager::SetMaxActivePointLights(int maxLights)
    {
        maxActivePointLights = std::clamp(maxLights, 0, static_cast<int>(kMaxGpuPointLights));
    }

    int LightManager::GetMaxActivePointLights() const
    {
        return maxActivePointLights;
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
        auto findById = [ID](const std::vector<Light *> &lights) -> Light *
        {
            auto it = std::find_if(lights.begin(), lights.end(), [ID](Light *light)
            {
                return light && light->GetID() == ID;
            });
            return it == lights.end() ? nullptr : *it;
        };

        if (Light *light = findById(static_point_lights_resources))
        {
            return light;
        }

        if (Light *light = findById(dynamic_point_lights_resources))
        {
            return light;
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
        std::vector<Light *> lights;
        lights.reserve(static_point_lights_resources.size() + dynamic_point_lights_resources.size());
        lights.insert(lights.end(), static_point_lights_resources.begin(), static_point_lights_resources.end());
        lights.insert(lights.end(), dynamic_point_lights_resources.begin(), dynamic_point_lights_resources.end());
        return lights;
    }

    void LightManager::InvalidateStaticPointLightCache()
    {
        staticPointLightsDirty = true;
    }

    void LightManager::Clear()
    {
        for (auto *light : static_point_lights_resources)
        {
            delete light;
        }
        for (auto *light : dynamic_point_lights_resources)
        {
            delete light;
        }

        static_point_lights_resources.clear();
        dynamic_point_lights_resources.clear();
        cached_static_point_lights.clear();
        lastActivePointLightIds.clear();
        spot_lights.clear();
        target_shader_resources.clear();
        nextEntityId = 0;
		staticPointLightsDirty = true;
		maxActivePointLights = static_cast<int>(kMaxGpuPointLights);
		RUNTIME_LIFECYCLE("Light manager", "cleared");
    }

}