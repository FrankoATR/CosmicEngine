#ifndef WAND_BODY_HPP
#define WAND_BODY_HPP

#define CALLBACK_COLLISION_EVENT(Method) [this](Object* other, BodyCollisionSide side) { this->Method(other, side); }


#include "../../Utils/Configurations.hpp"
#include <glm/glm.hpp>
#include <functional>

namespace WandEngine
{

	#if defined(GAME_2D_CONFIGURATION)
		enum class BodyType
		{
			Rectangle,
			Circle,
			Triangle
		};

	#elif defined(GAME_3D_CONFIGURATION)


	#else
		#error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
	#endif



    enum class BodyCollisionSide {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

	class Object;

	class Body
	{
	protected:
		int ID;
		bool active;
		BodyCollisionSide bodyType;

		Object *parent;
        bool aliveInGameManager;

		std::function<void(Object*, BodyCollisionSide)> onCollisionEvent;


		#if defined(GAME_2D_CONFIGURATION)
			glm::vec2 offSetParentPosition;
			glm::vec2 position;
			glm::vec2 size;
			float rotation; //Adaptar para las nuevas colisiones y OFFSET X,Y ?
		
		#elif defined(GAME_3D_CONFIGURATION)
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
		
        void OnCollision(Body *other, BodyCollisionSide side);

		void Draw();
		void Update();

		void SetID(int newBodyId);
		int GetID() const;

		void SetParent(Object *newParent);
		Object *GetParent() const;

		void SetOnCollisionEvent(std::function<void(Object*, BodyCollisionSide)> newOnCollionEvent);
		std::function<void(Object*, BodyCollisionSide)> GetOnCollisionEvent() const;

		void Destroy();
        bool IsAlive() const;


		#if defined(GAME_2D_CONFIGURATION)
			Body(Object *parent, glm::vec2 offSetParentPosition, glm::vec2 Size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent);

			glm::vec2 GetPosition();

			void SetOffSetParentPosition(glm::vec2 newOffSetParentPosition);
			glm::vec2 GetOffSetParentPosition();

			void SetSize(glm::vec2 NewSize);
			glm::vec2 GetSize();
		
		#elif defined(GAME_3D_CONFIGURATION)
			Body(Object *parent, glm::vec3 offSetParentPosition, glm::vec3 Size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent);

			glm::vec3 GetPosition();

			void SetOffSetParentPosition(glm::vec3 newOffSetParentPosition);
			glm::vec3 GetOffSetParentPosition();

			void SetSize(glm::vec3 NewSize);
			glm::vec3 GetSize();
        
        #else
            #error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
	};
}

#endif // WAND_BODY_HPP