/**
 * @file object.cpp
 * @brief Implements the base runtime object used by the engine entity system.
 */

#include "object.hpp"
#include "../timer/timer.hpp"

#include "../../managers/object/object_manager.hpp"
#include "../../managers/timer/timer_manager.hpp"
#include "../../utils/log.hpp"

#include <sstream>
#include <math.h>

namespace CosmicEngine
{

    void Object::draw() const
    {
        RUNTIME_WARNING("draw() Not implemented on " + className);
    }

    void Object::init()
    {
        RUNTIME_WARNING("init() Not implemented on " + className);
    }

    void Object::update(float deltaTime)
    {
        RUNTIME_WARNING("update() Not implemented on " + className);
    }

    std::string Object::GetClassName() const
    {
        return this->className;
    }

    void Object::SetID(int newObjectId)
    {
        this->ID = newObjectId;
    }

    int Object::GetID() const
    {
        return this->ID;
    }

    void Object::SetVisible(bool Visible)
    {
        this->visible = visible;
    }

    bool Object::GetVisible() const
    {
        return this->visible;
    }

    void Object::SetColor(glm::vec3 newColor)
    {
        this->mainColor = newColor;
    }
    
    glm::vec3 Object::GetColor() const
    {
        return this->mainColor;
    }
    
    /*
    bool Object::ReachPositionInTime(glm::vec2 NewPosition, double Duration, double DeltaTime)  // TODO IN SCHEDULE
    {
        if (VelocityForDuration == 0.0f)
        {
            float dx = NewPosition.x - Position.x;
            float dy = NewPosition.y - Position.y;
            float magnitude = sqrt(dx * dx + dy * dy);
            Direction.x = dx / magnitude;
            Direction.y = dy / magnitude;
            VelocityForDuration = magnitude / Duration;
        }
        if (fabs(NewPosition.x - Position.x) < 1 && fabs(NewPosition.y - Position.y) < 1)
        {
            VelocityForDuration = 0.0f;
            return true;
        }
        else
        {
            Position.x += Direction.x * VelocityForDuration * DeltaTime;
            Position.y += Direction.y * VelocityForDuration * DeltaTime;
            return false;
        }
    }

    bool Object::MoveForDirection(glm::vec2 NewDirection, double Duration, double DeltaTime)
    {
        if (!MovementTimer)
        {
            MovementTimer = new Timer(Duration, false, false);
            TimerManager::GetInstance().Add(MovementTimer);
            float dx = NewDirection.x - Position.x;
            float dy = NewDirection.y - Position.y;
            float magnitude = sqrt(dx * dx + dy * dy);
            Direction.x = dx / magnitude;
            Direction.y = dy / magnitude;
        }

        if(MovementTimer)
        {
            if (MovementTimer->IsTrigger())
            {
                MovementTimer = nullptr;
                return true;
            }
            else
            {
                UpdatePosition(DeltaTime);
                return false;
            }
        }
        else
        {
            return false;
        }
        // float radians = theta * (M_PI / 180.0f);
        // float directionX = cos(radians);
        // float directionY = sin(radians);
    }
    */


    void Object::SetInsideGridArea(bool insideGridArea)
    {
        this->insideGridArea = insideGridArea;
    }

    bool Object::GetInsideGridArea() const
    {
        return this->insideGridArea;
    }

    void Object::Destroy()
    {
        this->aliveInGameManager = false;
    }

    bool Object::IsAlive() const
    {
        return this->aliveInGameManager;
    }

    Object* Object::Clone() const
    {
        return new Object(*this);
    }

    void Object::unReferenceAll()
    {
        for(Object **copies : pointer_copies)
        {
            if (copies && *copies == this)
                *copies = nullptr;
        }
        pointer_copies.clear();
    }

    void Object::reset()
    {

    }

    std::vector<std::string> Object::GetAllValues() const
    {
        return {};
    }

    Object::~Object()
    {

    }


    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

        Object::Object(std::string className, glm::vec2 position, glm::vec2 size, float rotation, short int LayerId) : 
            className(className),
            mainColor(glm::vec3(1.0f)),
            //velocityForDuration(0.0f),
            //movementTimer(nullptr),
            aliveInGameManager(true),
            visible(true),
            insideGridArea(true),
            position(position), 
            size(size), 
            rotation(rotation),
            velocity(glm::vec2(0.0f)),
            maxVelocity(glm::vec2(3000.0f)),
            minVelocity(glm::vec2(-3000.0f)),
            angularVelocity(0.0f),
            acceleration(glm::vec2(0.0f)),
            lastPosition(position), 
            layerId(layerId)
        {

        }


        void Object::SetPosition(glm::vec2 newPosition)
        {
            this->lastPosition = this->position;
            this->position = newPosition;
        }
    
        glm::vec2 Object::GetPosition() const
        {
            return this->position;
        }
    
    
        glm::vec2 Object::GetLastPosition() const
        {
            return this->lastPosition;
        }
    
        void Object::UpdatePosition(float deltaTime)
        {
            lastPosition = position;
    
            velocity += acceleration * deltaTime;
    
            velocity = glm::clamp(velocity, minVelocity, maxVelocity);
    
            position += velocity * deltaTime;
            rotation += angularVelocity * deltaTime;
    
            rotation = glm::mod(rotation, 360.0f);
            if (rotation < 0.0f) rotation += 360.0f;
        }
    
    
        void Object::SetSize(glm::vec2 newSize)
        {
            this->size = newSize;
        }
    
        glm::vec2 Object::GetSize() const
        {
            return this->size;
        }
    
        void Object::SetLayerId(short int newLayerId)
        {
            this->layerId = newLayerId;
            CosmicEngine::ObjectManager::GetInstance().SortByLayer();
        }
    
        short int Object::GetLayerId() const
        {
            return this->layerId;
        }
    
        void Object::SetRotation(float newRotation)
        {
            this->rotation = newRotation;
        }
    
        float Object::GetRotation() const
        {
            return this->rotation;
        }
    
        void Object::SetVelocity(glm::vec2 newVelocity)
        {
            this->velocity = newVelocity;
        }
    
        glm::vec2 Object::GetVelocity() const
        {
            return this->velocity;
        }
    
    
        void Object::SetMaxVelocity(glm::vec2 newMaxVelocity)
        {
            maxVelocity = newMaxVelocity;
        }
    
        glm::vec2 Object::GetMaxVelocity() const
        {
            return maxVelocity;
        }
    
    
        void Object::SetMinVelocity(glm::vec2 newMinVelocity)
        {
            minVelocity = newMinVelocity;
        }
    
        glm::vec2 Object::GetMinVelocity() const
        {
            return minVelocity;
        }
        
        void Object::SetDirection(glm::vec2 newMinVelocity)
        {
            minVelocity = newMinVelocity;
        }

        glm::vec2 Object::GetDirection() const
        {
            return glm::vec2 ((glm::length(velocity) != 0.0f) ? glm::normalize(velocity) : glm::vec2(0.0f));
        }
    
    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        Object::Object(std::string className, glm::vec3 position, glm::vec3 size, glm::vec3 rotation) : 
            className(className),
            mainColor(glm::vec3(1.0f)),
            //velocityForDuration(0.0f),
            //movementTimer(nullptr),
            aliveInGameManager(true),
            visible(true),
            insideGridArea(true),
            position(position), 
            size(size), 
            rotation(rotation),
            velocity(glm::vec3(0.0f)),
            maxVelocity(glm::vec3(3000.0f)),
            minVelocity(glm::vec3(-3000.0f)),
            angularVelocity(0.0f),
            acceleration(glm::vec3(0.0f)),
            lastPosition(position)
        {

        }


        void Object::SetPosition(glm::vec3 newPosition)
        {
            this->lastPosition = this->position;
            this->position = newPosition;
        }

        glm::vec3 Object::GetPosition() const
        {
            return this->position;
        }


        glm::vec3 Object::GetLastPosition() const
        {
            return this->lastPosition;
        }

        void Object::UpdatePosition(float deltaTime)
        {
            lastPosition = position;

            velocity += acceleration * deltaTime;

            velocity = glm::clamp(velocity, minVelocity, maxVelocity);

            position += velocity * deltaTime;
            rotation += angularVelocity * deltaTime;

            rotation.x = glm::mod(rotation.x, 360.0f);
            if (rotation.x < 0.0f) rotation.x += 360.0f;
            
            rotation.y = glm::mod(rotation.y, 360.0f);
            if (rotation.y < 0.0f) rotation.y += 360.0f;
            
            rotation.z = glm::mod(rotation.z, 360.0f);
            if (rotation.z < 0.0f) rotation.z += 360.0f;
            
        }


        void Object::SetSize(glm::vec3 newSize)
        {
            this->size = newSize;
        }

        glm::vec3 Object::GetSize() const
        {
            return this->size;
        }

        void Object::SetRotation(glm::vec3 newRotation)
        {
            this->rotation = newRotation;
        }

        glm::vec3 Object::GetRotation() const
        {
            return this->rotation;
        }

        void Object::SetVelocity(glm::vec3 newVelocity)
        {
            this->velocity = newVelocity;
        }

        glm::vec3 Object::GetVelocity() const
        {
            return this->velocity;
        }


        void Object::SetMaxVelocity(glm::vec3 newMaxVelocity)
        {
            maxVelocity = newMaxVelocity;
        }

        glm::vec3 Object::GetMaxVelocity() const
        {
            return maxVelocity;
        }


        void Object::SetMinVelocity(glm::vec3 newMinVelocity)
        {
            minVelocity = newMinVelocity;
        }

        glm::vec3 Object::GetMinVelocity() const
        {
            return minVelocity;
        }

        void Object::SetDirection(glm::vec3 newMinVelocity)
        {
            minVelocity = newMinVelocity;
        }

        glm::vec3 Object::GetDirection() const
        {
            return glm::vec3 ((glm::length(velocity) != 0.0f) ? glm::normalize(velocity) : glm::vec3(0.0f));
        }

    
    #else
        #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif

}
