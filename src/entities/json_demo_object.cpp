#include "json_demo_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/input/input_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/managers/storage/json/json_manager.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>

const std::string &JsonDemoObject::StaticClassName()
{
    static const std::string className = "JsonDemoObject";
    return className;
}

JsonDemoObject::JsonDemoObject(glm::vec2 position, glm::vec2 size, const std::string &label, int health, short int layerId)
    : Object(StaticClassName(), position, size, 0.0f, layerId),
      label(label),
      health(health),
      moveSpeed(220.0f)
{
}

void JsonDemoObject::draw() const
{
    RS_MN.Render2DSprite("test_texture2", position, size);
    //RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), GetColor(), 1.0f, 2.0f);
    RS_MN.RenderText(label + " HP: " + std::to_string(health), "test_font", {position.x, position.y - 30.0f, 0.0f}, {0.45f, 0.45f, 1.0f});
}

void JsonDemoObject::update(float deltaTime)
{
    glm::vec2 movement(0.0f);

    if (INP_MN.IsKeyPressed(GLFW_KEY_LEFT, CosmicEngine::KeyRelease))
    {
        movement.x -= 1.0f;
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_RIGHT, CosmicEngine::KeyRelease))
    {
        movement.x += 1.0f;
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_UP, CosmicEngine::KeyRelease))
    {
        movement.y -= 1.0f;
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_DOWN, CosmicEngine::KeyRelease))
    {
        movement.y += 1.0f;
    }

    if (movement != glm::vec2(0.0f))
    {
        SetPosition(GetPosition() + glm::normalize(movement) * moveSpeed * deltaTime);
    }
}

CosmicEngine::Object *JsonDemoObject::Clone() const
{
    return new JsonDemoObject(*this);
}

std::vector<std::string> JsonDemoObject::GetAllValues() const
{
    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();

    return {
        "label=" + label,
        "health=" + std::to_string(health),
        "position=(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")",
        "size=(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ")"
    };
}

void JsonDemoObject::SetLabel(const std::string &newLabel)
{
    label = newLabel;
}

const std::string &JsonDemoObject::GetLabel() const
{
    return label;
}

void JsonDemoObject::SetHealth(int newHealth)
{
    health = std::max(0, newHealth);
}

int JsonDemoObject::GetHealth() const
{
    return health;
}

void JsonDemoObject::RegisterJsonSerialization()
{
    CosmicEngine::JsonManager::GetInstance().RegisterSerialization(
        StaticClassName(),
        {"label", "health", "position", "size", "rotation", "layer_id", "color"},
        Serialize,
        Deserialize
    );
}

nlohmann::json JsonDemoObject::Serialize(const CosmicEngine::Object *object)
{
    const JsonDemoObject *demoObject = dynamic_cast<const JsonDemoObject *>(object);
    if (!demoObject)
    {
        return nullptr;
    }

    glm::vec2 position = demoObject->GetPosition();
    glm::vec2 size = demoObject->GetSize();
    glm::vec3 color = demoObject->GetColor();

    return {
        {"label", demoObject->GetLabel()},
        {"health", demoObject->GetHealth()},
        {"position", {{"x", position.x}, {"y", position.y}}},
        {"size", {{"x", size.x}, {"y", size.y}}},
        {"rotation", demoObject->GetRotation()},
        {"layer_id", demoObject->GetLayerId()},
        {"color", {color.r, color.g, color.b}}
    };
}

CosmicEngine::Object *JsonDemoObject::Deserialize(const nlohmann::json &data)
{
    glm::vec2 position(
        data.value("position", nlohmann::json::object()).value("x", 0.0f),
        data.value("position", nlohmann::json::object()).value("y", 0.0f)
    );

    glm::vec2 size(
        data.value("size", nlohmann::json::object()).value("x", 96.0f),
        data.value("size", nlohmann::json::object()).value("y", 96.0f)
    );

    JsonDemoObject *demoObject = new JsonDemoObject(
        position,
        size,
        data.value("label", std::string("Objeto JSON")),
        data.value("health", 100),
        data.value("layer_id", 0)
    );

    demoObject->SetRotation(data.value("rotation", 0.0f));

    if (data.contains("color") && data["color"].is_array() && data["color"].size() == 3)
    {
        demoObject->SetColor({
            data["color"][0].get<float>(),
            data["color"][1].get<float>(),
            data["color"][2].get<float>()
        });
    }

    return demoObject;
}