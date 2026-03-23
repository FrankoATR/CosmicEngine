#include "json_demo_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/body/body_manager.hpp>
#include <CosmicEngine/managers/input/input_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/managers/timer/timer_manager.hpp>
#include <CosmicEngine/managers/storage/json/json_manager.hpp>
#include <CosmicEngine/models/body/body.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <random>

const std::string &JsonDemoObject::StaticClassName()
{
    static const std::string className = "JsonDemoObject";
    return className;
}

glm::vec2 JsonDemoObject::movementAreaPosition = glm::vec2(-480.0f, -270.0f);
glm::vec2 JsonDemoObject::movementAreaSize = glm::vec2(960.0f, 540.0f);

void JsonDemoObject::SetMovementArea(glm::vec2 position, glm::vec2 size)
{
    movementAreaPosition = position;
    movementAreaSize = size;
}

JsonDemoObject::JsonDemoObject(glm::vec2 position, glm::vec2 size, const std::string &label, int health, short int layerId)
    : Object(StaticClassName(), position, size, 0.0f, layerId),
      label(label),
      health(health),
      moveSpeed(95.0f),
      bodyId(-1),
            directionChangeTimer(nullptr),
      currentDirection(glm::vec2(1.0f, 0.0f))
{
    CreateBody();
    PickRandomDirection();
        ResetDirectionTimer();
}

JsonDemoObject::~JsonDemoObject()
{
    if (bodyId >= 0)
    {
        BOD_MN.Remove(bodyId);
        bodyId = -1;
    }

    if (directionChangeTimer)
    {
        directionChangeTimer->Destroy();
        directionChangeTimer = nullptr;
    }
}

void JsonDemoObject::CreateBody()
{
    CosmicEngine::Body *body = new CosmicEngine::Body(
        this,
        glm::vec2(0.0f, 0.0f),
        GetSize(),
        CALLBACK_COLLISION_EVENT(OnBodyCollision));

    BOD_MN.Add(body);
    bodyId = body->GetID();
}

void JsonDemoObject::PickRandomDirection()
{
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<float> angleDistribution(0.0f, 6.28318530718f);

    float angle = angleDistribution(generator);
    currentDirection = glm::normalize(glm::vec2(std::cos(angle), std::sin(angle)));
}

void JsonDemoObject::ResetDirectionTimer()
{
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<double> timerDistribution(1.0, 2.5);

    double nextDirectionTime = timerDistribution(generator);

    if (!directionChangeTimer)
    {
        directionChangeTimer = new CosmicEngine::Timer(nextDirectionTime, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(directionChangeTimer);
        return;
    }

    directionChangeTimer->SetTargetTime(nextDirectionTime);
    directionChangeTimer->reset();
    directionChangeTimer->Play();
}

void JsonDemoObject::KeepInsideMovementArea()
{
    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();
    glm::vec2 minPosition = movementAreaPosition;
    glm::vec2 maxPosition = movementAreaPosition + movementAreaSize - size;

    bool changedDirection = false;

    if (position.x < minPosition.x)
    {
        position.x = minPosition.x;
        currentDirection.x = std::abs(currentDirection.x);
        changedDirection = true;
    }
    else if (position.x > maxPosition.x)
    {
        position.x = maxPosition.x;
        currentDirection.x = -std::abs(currentDirection.x);
        changedDirection = true;
    }

    if (position.y < minPosition.y)
    {
        position.y = minPosition.y;
        currentDirection.y = std::abs(currentDirection.y);
        changedDirection = true;
    }
    else if (position.y > maxPosition.y)
    {
        position.y = maxPosition.y;
        currentDirection.y = -std::abs(currentDirection.y);
        changedDirection = true;
    }

    if (changedDirection)
    {
        SetPosition(position);
    }
}

void JsonDemoObject::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
{
    (void)side;

    if (!other)
    {
        return;
    }

    if (other->GetClassName() == StaticClassName())
    {
        Destroy();
        other->Destroy();
    }
}

void JsonDemoObject::draw() const
{
    RS_MN.Render2DSprite("test_texture2", position, size);
    //RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), GetColor(), 1.0f, 2.0f);
    RS_MN.RenderText(label + " HP: " + std::to_string(health), "test_font", {position.x, position.y - 30.0f, 0.0f}, {0.45f, 0.45f, 1.0f});
}

void JsonDemoObject::update(float deltaTime)
{
    if (directionChangeTimer && directionChangeTimer->IsTrigger())
    {
        PickRandomDirection();
        ResetDirectionTimer();
    }

    SetVelocity(currentDirection * moveSpeed);
    KeepInsideMovementArea();
}

CosmicEngine::Object *JsonDemoObject::Clone() const
{
    JsonDemoObject *clone = new JsonDemoObject(GetPosition(), GetSize(), label, health, GetLayerId());
    clone->SetColor(GetColor());
    clone->SetRotation(GetRotation());
    return clone;
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