#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#define IF_GET_TYPE(var, Type, target) \
    if (Type* var = static_cast<Type*>(target)) \
        if (var->GetClassName() == std::string(#Type))

#include <sqlite/sqlite3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <string>

namespace WandEngine
{

    class GameTimer;

    class GameObject
    {
    private:
        GameTimer *MovementTimer;
        float VelocityForDuration;

    protected:
        std::string ClassName;
        int ID;
        glm::vec2 Position;
        glm::vec2 LastPosition;
        glm::vec2 ViewDirection;
        glm::vec2 Size;
        short int LayerId; // 0-254  char

        float Rotation;
        float AngularVelocity;

        glm::vec2 Velocity;
        glm::vec2 MaxVelocity;
        glm::vec2 MinVelocity;
        
        glm::vec2 Acceleration;

        // Utilizar vectores para indicar la direccion de visualizacion glm::vec2
        bool Visible;
        bool InsideGridArea;
        bool AliveInGameManager;

        glm::vec3 MainColor;
        float Transparency;

/*
        GameObject &operator=(GameObject *other) // PROBAR AL REVES EN EL O1
        {
            if (this != other)
            {
                other->pointer_copies.push_back(&other);
            }
            return *this;
        }
*/


    public:

        std::vector<GameObject**> pointer_copies;


        GameObject() = delete;
        //GameObject(GameObject *Other);
        GameObject(std::string ClassName, glm::vec2 Position, glm::vec2 Size, float Rotation, short int LayerId);
        virtual void Draw();
        virtual void Init();
        virtual void Update(float deltaTime);

        void SetPosition(glm::vec2 NewPosition);
        glm::vec2 GetPosition() const;

        void UpdatePosition(float DeltaTime);

        void SetSize(glm::vec2 NewSize);
        glm::vec2 GetSize() const;

        std::string GetClassName() const;
        void SetClassName(std::string NewName);

        void SetID(int NewObjectId);
        int GetID() const;

        void SetLayerId(short int NewLayerId);
        short int GetLayerId() const;

        void UpdateLastPosition();
        glm::vec2 GetLastPosition() const;

        void SetVisible(bool Visible);
        bool GetVisible() const;

        void SetColor(glm::vec3 color);
        glm::vec3 GetColor() const;

        glm::vec2 GetDirection() const;

        void SetRotation(float NewRotation);
        float GetRotation() const;

        void SetVelocity(glm::vec2 NewVelocity);
        glm::vec2 GetVelocity() const;

        void SetMaxVelocity(glm::vec2 NewMaxVelocity);
        glm::vec2 GetMaxVelocity() const;

        void SetMinVelocity(glm::vec2 NewMinVelocity);
        glm::vec2 GetMinVelocity() const;

        bool ReachPositionInTime(glm::vec2 NewPosition, double Duration, double DeltaTime);
        bool MoveForDirection(glm::vec2 NewDirection, double Duration, double DeltaTime);

        void SetInsideGridArea(bool InsideGridArea);
        bool GetInsideGridArea() const;

        void Destroy();
        bool GetAliveInGameManager() const;

        virtual GameObject *Clone() const;

/*
        template <typename Derived>
        Derived* Clone() const {
            static_assert(std::is_base_of<GameObject, Derived>::value, 
                          "Clone Error");
            return new Derived(static_cast<const Derived&>(*this));
        }
*/

        template<typename TDerived>
        TDerived* makeReference(TDerived** outPtr) {
            pointer_copies.push_back(reinterpret_cast<GameObject**>(outPtr));
            *outPtr = static_cast<TDerived*>(this);
            return static_cast<TDerived*>(this);
        }

        template<typename TDerived>
        void unRerence(TDerived** outPtr)
        {
            auto it = std::find_if(pointer_copies.begin(), pointer_copies.end(), [outPtr](GameObject** copie) {
                return copie && *copie == reinterpret_cast<GameObject*>(*outPtr);
            });

            if(it != pointer_copies.end())
            {
                *outPtr = nullptr;
                pointer_copies.erase(it);
            }
        }

        virtual void Reset();


        virtual ~GameObject();
    };

}

#endif // GAMEOBJECT_HPP