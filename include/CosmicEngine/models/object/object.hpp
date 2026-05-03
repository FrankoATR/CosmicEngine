/**
 * @file object.hpp
 * @brief Declares the base object type used by the engine entity system.
 */

#ifndef COSMIC_OBJECT_HPP
#define COSMIC_OBJECT_HPP

/**
 * @brief Type-safe downcast guard that opens a scope when @p target is of the requested @p Type.
 *
 * Casts @p target to <tt>Type*</tt> and binds the result to @p var, then guards
 * the following block with a runtime check on @c GetClassName so the body only
 * executes when the dynamic type matches.
 *
 * @code
 * IF_GET_TYPE(npc, Enemy, hitObject) {
 *     npc->TakeDamage(10);
 * }
 * @endcode
 *
 * @param var    Local pointer name introduced inside the if-scope.
 * @param Type   Concrete object type to test against.
 * @param target Pointer to the candidate Object instance.
 */
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
     * @brief Base class for world objects managed by the engine.
     *
     * Object abstracts transform (position, size, rotation, velocity), visibility,
     * motion integration, lifecycle flags (alive/destroyed), and cloning helpers
     * shared by both 2D and 3D runtime objects.  Derive your entity classes from
     * Object, override draw()/update(), and register each instance with the
     * ObjectManager via OBJ_MN.Add().
     *
    * @par Example - creating a 2D game entity (GAME_2D_CONFIGURATION)
     * @code
     * class JsonDemoObject : public CosmicEngine::Object
     * {
     * public:
     *     JsonDemoObject(glm::vec2 position, glm::vec2 size)
    *         : Object("JsonDemoObject", position, size, 0.0f, 1)
     *     {}
     *
     *     void update(float deltaTime) override
     *     {
     *         SetVelocity(glm::vec2(95.0f, 0.0f));  // move right
     *     }
     *
     *     void draw() const override
     *     {
    *         RS_MN.Render2DSpriteUnlit("test_texture",
     *             GetPosition(), GetSize(), GetRotation(), GetColor());
     *     }
     * };
     *
     * // Register the object with the runtime:
     * auto* obj = new JsonDemoObject({100.0f, 200.0f}, {96.0f, 96.0f});
     * obj->SetColor({0.2f, 0.9f, 0.4f});
     * OBJ_MN.Add(obj);
     * @endcode
     *
    * @par Example - creating a 3D game entity (GAME_3D_CONFIGURATION)
     * @code
     * class CollisionDemoObject3D : public CosmicEngine::Object
     * {
     * public:
     *     CollisionDemoObject3D(glm::vec3 position, glm::vec3 size)
     *         : Object("CollisionDemoObject3D", position, size, glm::vec3(0.0f))
     *     {}
     * };
     *
     * auto* obj3d = new CollisionDemoObject3D({0,0,0}, {2,2,2});
     * OBJ_MN.Add(obj3d);
     * @endcode
     */
    class Object
    {
    private:
        int ID;

        /**
         * @brief Integrates the current velocity into the object transform.
         * @param deltaTime Time step used for position integration.
         */
        void UpdatePosition(float deltaTime);

        friend ObjectManager;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION


        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
        
    protected:
        std::string className;                ///< Logical class name reported by GetClassName.
        bool visible;                         ///< When false the object is skipped during draw.
        bool insideGridArea;                  ///< Tracks whether the object is currently inside a CollisionArea grid.
        bool aliveInGameManager;              ///< Tracks whether ObjectManager still references this object.
        float transparency;                   ///< Global alpha multiplier (0..1) applied at render time.
        glm::vec3 mainColor;                  ///< Base tint color used by the renderer.
        std::vector<Object**> pointer_copies; ///< External pointers cleared automatically when the object is destroyed.

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 position;               ///< World-space position (2D).
            glm::vec2 lastPosition;           ///< Position at the previous update tick (2D).
            glm::vec2 viewDirection;          ///< Forward / facing direction (2D).
            glm::vec2 size;                   ///< Width / height of the object (2D).
            short int layerId;                ///< Render layer (0-254).
            float rotation;                   ///< Rotation in degrees around Z (2D).
            float angularVelocity;            ///< Angular speed in degrees per second (2D).
            glm::vec2 acceleration;           ///< Linear acceleration applied to velocity (2D).

            glm::vec2 velocity;               ///< Current linear velocity (2D).
            glm::vec2 maxVelocity;            ///< Per-axis upper clamp applied to velocity (2D).
            glm::vec2 minVelocity;            ///< Per-axis lower clamp applied to velocity (2D).

            // Utilizar vectores para indicar la direccion de visualizacion glm::vec2

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 position;               ///< World-space position (3D).
            glm::vec3 lastPosition;           ///< Position at the previous update tick (3D).
            glm::vec3 viewDirection;          ///< Forward / facing direction (3D).
            glm::vec3 size;                   ///< Extents of the object along each axis (3D).
            glm::vec3 rotation;               ///< Euler angles in degrees (3D).
            glm::vec3 angularVelocity;        ///< Angular speed per axis in degrees per second (3D).
            glm::vec3 acceleration;           ///< Linear acceleration applied to velocity (3D).

            glm::vec3 velocity;               ///< Current linear velocity (3D).
            glm::vec3 maxVelocity;            ///< Per-axis upper clamp applied to velocity (3D).
            glm::vec3 minVelocity;            ///< Per-axis lower clamp applied to velocity (3D).
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    public:

        Object() = delete;

        /** @brief Draws the object. */
        virtual void draw() const;
        
        /** @brief Performs object-specific initialization logic. */
        virtual void init();

        /**
         * @brief Updates object-specific runtime logic.
         * @param deltaTime Fixed update time step.
         */
        virtual void update(float deltaTime);

        /**
         * @brief Sets the unique identifier assigned by the object manager.
         * @param newID New object identifier.
         */
        void SetID(int newID);

        /**
         * @brief Returns the current object identifier.
         * @return Object identifier.
         */
        int GetID() const;

        /**
         * @brief Returns the logical class name assigned to the object.
         * @return Class name.
         */
        std::string GetClassName() const;

        /**
         * @brief Sets whether the object should be rendered.
         * @param visible New visibility state.
         */
        void SetVisible(bool visible);

        /**
         * @brief Returns whether the object is visible.
         * @return True when the object is marked as visible.
         */
        bool GetVisible() const;

        /**
         * @brief Sets the main display color of the object.
         * @param color New object color.
         */
        void SetColor(glm::vec3 color);
        /**
         * @brief Returns the main display color of the object.
         * @return Object color.
         */
        glm::vec3 GetColor() const;

        //bool ReachPositionInTime(glm::vec2 newPosition, double duration, double deltaTime);
        //bool MoveForDirection(glm::vec2 newDirection, double duration, double deltaTime);

        /**
         * @brief Marks whether the object is currently inside the active grid area.
         * @param insideGridArea Grid-area membership flag.
         */
        void SetInsideGridArea(bool insideGridArea);
        /**
         * @brief Returns whether the object is currently inside the active grid area.
         * @return True when the object belongs to the active grid area.
         */
        bool GetInsideGridArea() const;

        /** @brief Marks the object for destruction. */
        void Destroy();
        /**
         * @brief Returns whether the object is still alive in the runtime.
         * @return True when the object has not been destroyed.
         */
        bool IsAlive() const;

        /**
         * @brief Creates a copy of the object.
         * @return Newly allocated object clone.
         */
        virtual Object *Clone() const;

        /**
         * @brief Registers an external pointer as a live reference to this object.
         * @tparam TDerived Expected object type.
         * @param outPtr Pointer that should mirror this object instance.
         * @return Typed pointer to this object.
         */
        template<typename TDerived>
        TDerived* makeReference(TDerived** outPtr) {
            pointer_copies.push_back(reinterpret_cast<Object**>(outPtr));
            *outPtr = static_cast<TDerived*>(this);
            return static_cast<TDerived*>(this);
        }

        /**
         * @brief Removes a previously registered external reference.
         * @tparam TDerived Referenced object type.
         * @param outPtr External pointer to clear.
         */
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

        /** @brief Clears every registered external reference to the object. */
        void unReferenceAll();
        
        /** @brief Restores the object state to its reset condition. */
        virtual void reset();

        /**
         * @brief Serializes the object state into a string vector.
         * @return Collection of serialized values.
         */
        virtual std::vector<std::string> GetAllValues() const;

        /** @brief Releases the object and its tracked references. */
        virtual ~Object();

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            /**
             * @brief Creates a 2D object.
             * @param className Logical runtime class name.
             * @param position Initial world position.
             * @param size Initial size.
             * @param rotation Initial rotation.
             * @param LayerId Initial rendering layer identifier.
             */
            Object(std::string className, glm::vec2 position, glm::vec2 size, float rotation = 0.0f, short int LayerId = 0);

            /**
             * @brief Sets the 2D position of the object.
             * @param newPosition Value provided by the caller.
             */
            void SetPosition(glm::vec2 newPosition);
            /**
             * @brief Returns the 2D position of the object.
             * @return The 2D position of the object.
             */
            glm::vec2 GetPosition() const;

            /**
             * @brief Returns the previous 2D position stored by the object.
             * @return The previous 2D position stored by the object.
             */
            glm::vec2 GetLastPosition() const;

            /**
             * @brief Sets the 2D size of the object.
             * @param newSize Value provided by the caller.
             */
            void SetSize(glm::vec2 newSize);
            /**
             * @brief Returns the 2D size of the object.
             * @return The 2D size of the object.
             */
            glm::vec2 GetSize() const;

            /**
             * @brief Sets the rendering layer identifier.
             * @param newLayerId Value provided by the caller.
             */
            void SetLayerId(short int newLayerId);
            /**
             * @brief Returns the rendering layer identifier.
             * @return The rendering layer identifier.
             */
            short int GetLayerId() const;

            /**
             * @brief Sets the 2D rotation value.
             * @param newRotation Value provided by the caller.
             */
            void SetRotation(float newRotation);
            /**
             * @brief Returns the 2D rotation value.
             * @return The 2D rotation value.
             */
            float GetRotation() const;

            /**
             * @brief Sets the 2D velocity.
             * @param newVelocity Value provided by the caller.
             */
            void SetVelocity(glm::vec2 newVelocity);
            /**
             * @brief Returns the 2D velocity.
             * @return The 2D velocity.
             */
            glm::vec2 GetVelocity() const;

            /**
             * @brief Sets the 2D maximum velocity clamp.
             * @param newMaxVelocity Value provided by the caller.
             */
            void SetMaxVelocity(glm::vec2 newMaxVelocity);
            /**
             * @brief Returns the 2D maximum velocity clamp.
             * @return The 2D maximum velocity clamp.
             */
            glm::vec2 GetMaxVelocity() const;

            /**
             * @brief Sets the 2D minimum velocity clamp.
             * @param newMinVelocity Value provided by the caller.
             */
            void SetMinVelocity(glm::vec2 newMinVelocity);
            /**
             * @brief Returns the 2D minimum velocity clamp.
             * @return The 2D minimum velocity clamp.
             */
            glm::vec2 GetMinVelocity() const;

            /**
             * @brief Sets the 2D facing direction.
             * @param newDirection Value provided by the caller.
             */
            void SetDirection(glm::vec2 newDirection);
            /**
             * @brief Returns the 2D facing direction.
             * @return The 2D facing direction.
             */
            glm::vec2 GetDirection() const;

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            /**
             * @brief Creates a 3D object.
             * @param className Logical runtime class name.
             * @param position Initial world position.
             * @param size Initial size.
             * @param rotation Initial Euler rotation.
             */
            Object(std::string className, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f));

            /**
             * @brief Sets the 3D position of the object.
             * @param newPosition Value provided by the caller.
             */
            void SetPosition(glm::vec3 newPosition);
            /**
             * @brief Returns the 3D position of the object.
             * @return The 3D position of the object.
             */
            glm::vec3 GetPosition() const;

            /**
             * @brief Returns the previous 3D position stored by the object.
             * @return The previous 3D position stored by the object.
             */
            glm::vec3 GetLastPosition() const;

            /**
             * @brief Sets the 3D size of the object.
             * @param newSize Value provided by the caller.
             */
            void SetSize(glm::vec3 newSize);
            /**
             * @brief Returns the 3D size of the object.
             * @return The 3D size of the object.
             */
            glm::vec3 GetSize() const;

            /**
             * @brief Sets the 3D Euler rotation of the object.
             * @param newRotation Value provided by the caller.
             */
            void SetRotation(glm::vec3 newRotation);
            /**
             * @brief Returns the 3D Euler rotation of the object.
             * @return The 3D Euler rotation of the object.
             */
            glm::vec3 GetRotation() const;

            /**
             * @brief Sets the 3D velocity.
             * @param newVelocity Value provided by the caller.
             */
            void SetVelocity(glm::vec3 newVelocity);
            /**
             * @brief Returns the 3D velocity.
             * @return The 3D velocity.
             */
            glm::vec3 GetVelocity() const;

            /**
             * @brief Sets the 3D maximum velocity clamp.
             * @param newMaxVelocity Value provided by the caller.
             */
            void SetMaxVelocity(glm::vec3 newMaxVelocity);
            /**
             * @brief Returns the 3D maximum velocity clamp.
             * @return The 3D maximum velocity clamp.
             */
            glm::vec3 GetMaxVelocity() const;

            /**
             * @brief Sets the 3D minimum velocity clamp.
             * @param newMinVelocity Value provided by the caller.
             */
            void SetMinVelocity(glm::vec3 newMinVelocity);
            /**
             * @brief Returns the 3D minimum velocity clamp.
             * @return The 3D minimum velocity clamp.
             */
            glm::vec3 GetMinVelocity() const;

            /**
             * @brief Sets the 3D facing direction.
             * @param newDirection Value provided by the caller.
             */
            void SetDirection(glm::vec3 newDirection);
            /**
             * @brief Returns the 3D facing direction.
             * @return The 3D facing direction.
             */
            glm::vec3 GetDirection() const;
        
        #else
            #error "[Object] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };






}

#endif // COSMIC_OBJECT_HPP
