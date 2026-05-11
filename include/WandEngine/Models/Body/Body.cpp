#include "Body.hpp"

#include "../../Managers/Resource/ResourceManager.hpp"
#include "../Object/Object.hpp"
#include "../../Utils/Log.hpp"

namespace WandEngine
{

	void Body::Update()
	{
		position = parent->GetPosition() + offSetParentPosition;
	}
	
    void Body::OnCollision(Body *other, BodyCollisionSide side)
    {
		if(onCollisionEvent && parent)
		{
			onCollisionEvent(other->GetParent(), side);
		}
    }

	void Body::SetID(int NewBodyId)
	{
		this->ID = NewBodyId;
	}

	int Body::GetID() const
	{
		return this->ID;
	}

	void Body::SetParent(Object *newParent)
	{
		this->parent = newParent;
	}

	Object *Body::GetParent() const
	{
		return this->parent;
	}

	void Body::SetOnCollisionEvent(std::function<void(Object*, BodyCollisionSide)> newOnCollionEvent)
	{
		onCollisionEvent = newOnCollionEvent;
	}

	std::function<void(Object*, BodyCollisionSide)> Body::GetOnCollisionEvent() const
	{
		return this->onCollisionEvent;
	}


    void Body::Destroy()
    {
        this->aliveInGameManager = false;
    }

	bool Body::IsAlive() const
    {
        return this->aliveInGameManager;
    }


	#if defined(GAME_2D_CONFIGURATION)
		Body::Body(Object *parent, glm::vec2 offSetParentPosition, glm::vec2 size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent) : 
			parent(parent), active(true), onCollisionEvent(onCollisionEvent), aliveInGameManager(true), offSetParentPosition(offSetParentPosition), size(size)
		{
			if(!parent)
			{
				RUNTIME_WARNING("Parent NOT FOUND for this Body");
			}
			if(!onCollisionEvent)
			{
				RUNTIME_WARNING("OnCollisionEvent NOT FOUND for this Body");
			}

			position = parent->GetPosition() + offSetParentPosition;

		}

		void Body::Draw()
		{
			ResourceManager::GetInstance().RenderRectangle(
				glm::vec3(position, 0.0f), 
				glm::vec3(position + size, 0.0f), 
				glm::vec3(position.x + size.x / 2, position.y + size.y / 2, 0.0f), 
				glm::vec3(0.0f, 0.0f, 0.0f), 
				glm::vec3(1.0f, 0.0f, 0.0f), 
				1.0f, 
				2.0f);
		}
	
		glm::vec2 Body::GetPosition()
		{
			return this->position;
		}
	
		void Body::SetOffSetParentPosition(glm::vec2 newOffSetParentPosition)
		{
			this->offSetParentPosition = newOffSetParentPosition;
		}
	
		glm::vec2 Body::GetOffSetParentPosition()
		{
			return this->offSetParentPosition;
		}
	
		void Body::SetSize(glm::vec2 newSize)
		{
			this->size = newSize;
		}
	
		glm::vec2 Body::GetSize()
		{
			return this->size;
		}
	

	#elif defined(GAME_3D_CONFIGURATION)
		Body::Body(Object *parent, glm::vec3 offSetParentPosition, glm::vec3 size, std::function<void(Object*, BodyCollisionSide)> onCollisionEvent) : 
			parent(parent), active(true), onCollisionEvent(onCollisionEvent), aliveInGameManager(true), offSetParentPosition(offSetParentPosition), size(size)
		{
			if(!parent)
			{
				RUNTIME_WARNING("Parent NOT FOUND for this Body");
			}
			if(!onCollisionEvent)
			{
				RUNTIME_WARNING("OnCollisionEvent NOT FOUND for this Body");
			}

			position = parent->GetPosition() + offSetParentPosition;

		}

		void Body::Draw()
		{
			ResourceManager::GetInstance().RenderRectangle(
				glm::vec3(position), 
				glm::vec3(position + size), 
				glm::vec3(position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2), 
				glm::vec3(0.0f, 0.0f, 0.0f), 
				glm::vec3(1.0f, 0.0f, 0.0f), 
				1.0f, 
				2.0f);
		}

		glm::vec3 Body::GetPosition()
		{
			return this->position;
		}

		void Body::SetOffSetParentPosition(glm::vec3 newOffSetParentPosition)
		{
			this->offSetParentPosition = newOffSetParentPosition;
		}

		glm::vec3 Body::GetOffSetParentPosition()
		{
			return this->offSetParentPosition;
		}

		void Body::SetSize(glm::vec3 newSize)
		{
			this->size = newSize;
		}

		glm::vec3 Body::GetSize()
		{
			return this->size;
		}


	#else
		#error "[Body] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
	#endif


}