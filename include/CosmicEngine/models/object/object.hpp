/**
 * @file object.hpp
 * @brief Contains Code for Object (Entities)
 * 
 * This file include all 2D and 3D configurations for Objects
 * 
 * @author Francisco Rosa
 * @date 2025-04-21
 * @version 1.0
 * @copyright MIT License
 */

#ifndef COSMIC_OBJECT_HPP
#define COSMIC_OBJECT_HPP

#define IF_GET_TYPE(var, Type, target) \
    if (Type* var = static_cast<Type*>(target)) \
        if (var->GetClassName() == std::string(#Type))

#include <sqlite/sqlite3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <string>

namespace CosmicEngine
{
    class ObjectManager;


    /**
     * @class Object
     * @brief Class with the methods needed to implement entities.
     */
    class Object
    {
    private:
        int ID;

        /**
        * @brief update physics logic
        */
       void UpdatePosition(float deltaTime);

        friend ObjectManager;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION


        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
        
    protected:
        std::string className;
        bool visible;
        bool insideGridArea;
        bool aliveInGameManager;
        float transparency;
        glm::vec3 mainColor;
        std::vector<Object**> pointer_copies;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 position;
            glm::vec2 lastPosition;
            glm::vec2 viewDirection;
            glm::vec2 size;
            short int layerId; // 0-254  char
            float rotation;
            float angularVelocity;
            glm::vec2 acceleration;

            glm::vec2 velocity;
            glm::vec2 maxVelocity;
            glm::vec2 minVelocity;

            // Utilizar vectores para indicar la direccion de visualizacion glm::vec2

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 position;
            glm::vec3 lastPosition;
            glm::vec3 viewDirection;
            glm::vec3 size;
            glm::vec3 rotation;
            glm::vec3 angularVelocity;
            glm::vec3 acceleration;

            glm::vec3 velocity;
            glm::vec3 maxVelocity;
            glm::vec3 minVelocity;
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    public:

        Object() = delete;

        /**
        * @brief draw the object on window.
        */
        virtual void draw() const;
        
        /**
        * @brief Initial configurations for the object.
        */
        virtual void init();

        /**
        * @brief Logic for the object.
        * @param deltaTime Value to maintain consistency in movement calculations within the logic.
        */
        virtual void update(float deltaTime);

        /**
        * @brief Set the unique ID for the object.
        * @param newID Changes the current ID of the object to this one.
        */
        void SetID(int newID);

        /**
        * @brief Gets the current ID value of the object.
        * @return Current ID of the Object.
        */
        int GetID() const;

        /**
        * @brief Gets the value of the object's class name.
        * @return Current className of the Object.
        */
        std::string GetClassName() const;

        /**
        * @brief Sets the visibility value of the object.
        * @param visible Must be true or false.
        * @example obj->SetVisible(true);
        */
        void SetVisible(bool visible);

        /**
        * @brief Gets whether the object is currently visible.
        * @return Current visible value of the Object.
        */
        bool GetVisible() const;

        void SetColor(glm::vec3 color);
        glm::vec3 GetColor() const;

        //bool ReachPositionInTime(glm::vec2 newPosition, double duration, double deltaTime);
        //bool MoveForDirection(glm::vec2 newDirection, double duration, double deltaTime);

        void SetInsideGridArea(bool insideGridArea);
        bool GetInsideGridArea() const;


        void Destroy();
        bool IsAlive() const;

        virtual Object *Clone() const;

        template<typename TDerived>
        TDerived* makeReference(TDerived** outPtr) {
            pointer_copies.push_back(reinterpret_cast<Object**>(outPtr));
            *outPtr = static_cast<TDerived*>(this);
            return static_cast<TDerived*>(this);
        }

        template<typename TDerived>
        void unRerence(TDerived** outPtr)
        {
            auto it = std::find_if(pointer_copies.begin(), pointer_copies.end(), [outPtr](Object** copie) {
                return copie && *copie == reinterpret_cast<Object*>(*outPtr);
            });

            if(it != pointer_copies.end())
            {
                *outPtr = nullptr;
                pointer_copies.erase(it);
            }
        }

        void unReferenceAll();
        
        virtual void reset();

        virtual std::vector<std::string> GetAllValues() const;

        virtual ~Object();

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            Object(std::string className, glm::vec2 position, glm::vec2 size, float rotation = 0.0f, short int LayerId = 0);

            void SetPosition(glm::vec2 newPosition);
            glm::vec2 GetPosition() const;

            glm::vec2 GetLastPosition() const;

            void SetSize(glm::vec2 newSize);
            glm::vec2 GetSize() const;

            void SetLayerId(short int newLayerId);
            short int GetLayerId() const;

            void SetRotation(float newRotation);
            float GetRotation() const;

            void SetVelocity(glm::vec2 newVelocity);
            glm::vec2 GetVelocity() const;

            void SetMaxVelocity(glm::vec2 newMaxVelocity);
            glm::vec2 GetMaxVelocity() const;

            void SetMinVelocity(glm::vec2 newMinVelocity);
            glm::vec2 GetMinVelocity() const;

            void SetDirection(glm::vec2 newDirection);
            glm::vec2 GetDirection() const;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            Object(std::string className, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f));

            void SetPosition(glm::vec3 newPosition);
            glm::vec3 GetPosition() const;

            glm::vec3 GetLastPosition() const;

            void SetSize(glm::vec3 newSize);
            glm::vec3 GetSize() const;

            void SetRotation(glm::vec3 newRotation);
            glm::vec3 GetRotation() const;

            void SetVelocity(glm::vec3 newVelocity);
            glm::vec3 GetVelocity() const;

            void SetMaxVelocity(glm::vec3 newMaxVelocity);
            glm::vec3 GetMaxVelocity() const;

            void SetMinVelocity(glm::vec3 newMinVelocity);
            glm::vec3 GetMinVelocity() const;

            void SetDirection(glm::vec3 newDirection);
            glm::vec3 GetDirection() const;
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };






}

#endif // COSMIC_OBJECT_HPP