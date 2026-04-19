#ifndef COSMIC_BODY_HPP
#define COSMIC_BODY_HPP

/**
 * @file body.hpp
 * @brief Declares collision body types and the base body component used by the engine.
 */

#define CALLBACK_COLLISION_EVENT(Method) \
    [this](CosmicEngine::Object* other, CosmicEngine::BodyCollisionSide side) \
    { \
        this->Method(other, side); \
    }

#include <glm/glm.hpp>
#include <functional>

namespace CosmicEngine
{

	#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
		/**
		 * @brief Supported 2D collision body shapes.
		 */
		enum class BodyType
		{
			Rectangle,
			Circle,
			Triangle
		};

	#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION


	#else
		#error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
	#endif


	/**
	 * @brief Identifies the side on which a collision occurred.
	 */
    enum class BodyCollisionSide {
        NONE,
        LEFT,
        RIGHT,
        TOP,
		BOTTOM,
		FRONT,
		BACK
    };

	class Object;

	/**
	 * @brief Represents a collision body attached to an object.
	 *
	 * A Body stores collision dimensions, parent-relative offsets, and a collision
	 * callback that is invoked when the spatial collision system detects contact.
	 * Bodies are managed by the BodyManager and automatically updated from their
	 * parent Object's transform every frame.
	 *
	 * @par Example — attaching a 2D collision body to an object
	 * @code
	 * void MyEntity::CreateBody()
	 * {
	 *     CosmicEngine::Body* body = new CosmicEngine::Body(
	 *         this,                              // parent object
	 *         glm::vec2(0.0f, 0.0f),             // offset from parent
	 *         GetSize(),                          // collision size
	 *         CALLBACK_COLLISION_EVENT(OnBodyCollision));
	 *
	 *     BOD_MN.Add(body);
	 *     bodyId = body->GetID();
	 * }
	 *
	 * void MyEntity::OnBodyCollision(CosmicEngine::Object* other,
	 *                                CosmicEngine::BodyCollisionSide side)
	 * {
	 *     if (other && other->GetClassName() == "Enemy")
	 *         Destroy();
	 * }
	 * @endcode
	 *
	 * @par Example — cleaning up the body in the destructor
	 * @code
	 * MyEntity::~MyEntity()
	 * {
	 *     if (bodyId >= 0)
	 *     {
	 *         BOD_MN.Remove(bodyId);
	 *         bodyId = -1;
	 *     }
	 * }
	 * @endcode
	 */
	class Body
	{
	protected:
		int ID;
		bool active;
		BodyCollisionSide bodyType;

		Object *parent;
        bool aliveInGameManager;

		std::function<void(Object*, BodyCollisionSide)> onCollisionEvent;


		#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
			glm::vec2 offSetParentPosition;
			glm::vec2 position;
			glm::vec2 size;
			float rotation; //Adaptar para las nuevas colisiones y OFFSET X,Y ?
		
		#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
			glm::vec3 offSetParentPosition;
			glm::vec3 position;
			glm::vec3 size;
			glm::vec3 rotation;
        
        #else
            #error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif



	public:
		Body() = delete;
		~Body() = default;
		
        /**
         * @brief Executes the registered collision callback.
         * @param other Colliding body.
         * @param side Collision side relative to this body.
         */
        void OnCollision(Body *other, BodyCollisionSide side);

		/** @brief Draws the collision body for debug visualization. */
		void draw();
		/** @brief Updates the collision body transform from its parent object. */
		void update();

		/** @brief Sets the unique body identifier assigned by the body manager. */
		void SetID(int newBodyId);
		/** @brief Returns the unique body identifier. */
		int GetID() const;

		/** @brief Sets the object that owns this collision body. */
		void SetParent(Object *newParent);
		/** @brief Returns the object that owns this collision body. */
		Object *GetParent() const;

		/**
		 * @brief Sets the callback invoked when this body collides with another object.
		 * @param newOnCollionEvent Collision callback.
		 */
		void SetOnCollisionEvent(std::function<void(Object*, BodyCollisionSide)> newOnCollionEvent);
		/** @brief Returns the callback invoked when this body collides with another object. */
		std::function<void(Object*, BodyCollisionSide)> GetOnCollisionEvent() const;

		/** @brief Marks the body for removal from the runtime. */
		void Destroy();
        /** @brief Returns whether the body is still alive in the runtime. */
        bool IsAlive() const;


		#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
			/**
			 * @brief Creates a 2D collision body.
			 * @param parent Object that owns the body.
			 * @param offSetParentPosition Offset relative to the parent object position.
			 * @param Size Body size.
			 * @param onCollisionEvent Collision callback.
			 */
			Body(Object *parent, glm::vec2 offSetParentPosition, glm::vec2 Size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent);

			/** @brief Returns the current world position of the 2D body. */
			glm::vec2 GetPosition();

			/** @brief Sets the parent-relative offset of the 2D body. */
			void SetOffSetParentPosition(glm::vec2 newOffSetParentPosition);
			/** @brief Returns the parent-relative offset of the 2D body. */
			glm::vec2 GetOffSetParentPosition();

			/** @brief Sets the 2D body size. */
			void SetSize(glm::vec2 NewSize);
			/** @brief Returns the 2D body size. */
			glm::vec2 GetSize();
		
		#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
			/**
			 * @brief Creates a 3D collision body.
			 * @param parent Object that owns the body.
			 * @param offSetParentPosition Offset relative to the parent object position.
			 * @param Size Body size.
			 * @param onCollisionEvent Collision callback.
			 */
			Body(Object *parent, glm::vec3 offSetParentPosition, glm::vec3 Size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent);

			/** @brief Returns the current world position of the 3D body. */
			glm::vec3 GetPosition();

			/** @brief Sets the parent-relative offset of the 3D body. */
			void SetOffSetParentPosition(glm::vec3 newOffSetParentPosition);
			/** @brief Returns the parent-relative offset of the 3D body. */
			glm::vec3 GetOffSetParentPosition();

			/** @brief Sets the 3D body size. */
			void SetSize(glm::vec3 NewSize);
			/** @brief Returns the 3D body size. */
			glm::vec3 GetSize();
        
        #else
            #error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
	};
}

#endif // COSMIC_BODY_HPP