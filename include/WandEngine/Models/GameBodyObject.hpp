#ifndef GAMEBODYOBJECT_HPP
#define GAMEBODYOBJECT_HPP


#include <glm/glm.hpp>
#include <functional>

namespace WandEngine
{

	enum class GameBodyObjectType
    {
        Body_Rectangle,
        Body_Circle,
        Body_Triangle
    };

    enum class CollisionSide {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

	class GameObject;

	class GameBodyObject
	{

	private:
		GameBodyObjectType BodyType;
		int ID;
		bool Active;
		glm::vec2 OffSetParentPosition;
		glm::vec2 Position;
		glm::vec2 Size;
		float Rotation = 0.0f; //Adaptar para las nuevas colisiones y OFFSET X,Y ?
		GameObject *Parent;
        bool AliveInGameManager;

		std::function<void(GameObject*, CollisionSide)> OnCollisionEvent;
	public:
		GameBodyObject() = delete;
		GameBodyObject(GameObject *Parent, glm::vec2 OffSetParentPosition, glm::vec2 Size, std::function<void(GameObject*, CollisionSide)> OnCollisionEvent);
        void OnCollision(GameBodyObject *other, CollisionSide Side);

		void UpdatePosition();
		glm::vec2 GetPosition();

		void SetOffSetParentPosition(glm::vec2 NewOffSetParentPosition);
		glm::vec2 GetOffSetParentPosition();


		void SetSize(glm::vec2 NewSize);
		glm::vec2 GetSize();

		void DrawBody();
		void DrawCoordinates();

		void SetID(int NewBodyId);
		int GetID();

		void SetParent(GameObject *NewParent);
		GameObject *GetParent();

		void SetOnCollisionEvent(std::function<void(GameObject*, CollisionSide)> newOnCollionEvent);
		std::function<void(GameObject*, CollisionSide)> GetOnCollisionEvent();

		void Destroy();
        bool GetAliveInGameManager() const;

		~GameBodyObject();
	};
}

#endif // GAMEBODYOBJECT_HPP