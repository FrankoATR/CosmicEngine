#include "Player.hpp"
#include "SolidBlock.hpp"
#include "Spike.hpp"
#include "Orb.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/Audio/Sound/SoundManager.hpp>

Player::Player(std::string UniqueName, PlayerMode mode, glm::vec2 position, glm::vec2 size, float rotation, short int LayerId) : 
    Object("Player", position, size, rotation, LayerId), CurrentPlayerMode(mode)
{
    OnGroundOrBlock = false;
    velocity.x = 1040.0f;
    velocity.y = 0.0f;
    acceleration.y = 9200.0f;
    angularVelocity = 0.0f;
    KeySpaceDown = false;
    RigthClickDown = false;
    KeySpaceRelease = false;
    RigthClickRelease = false;
    ReleaseJumping = false;
    this->UniqueName = UniqueName;
}

void Player::Init()
{
    body1 = new Body(this, glm::vec2(0.0f), GetSize(), CALLBACK_COLLISION_EVENT(Body1CollisionEvent));
    BodyManager::GetInstance().Add(body1);


    glm::vec2 collisionsize = {GetSize().x * 0.30f, GetSize().y * 0.30f};
    glm::vec2 collisionposition = {
        (GetSize().x - collisionsize.x) / 2,
        (GetSize().y - collisionsize.y) / 2
    };
    body2 = new Body(this, collisionposition, collisionsize, CALLBACK_COLLISION_EVENT(Body2CollisionEvent));
    BodyManager::GetInstance().Add(body2);


    TimeToEndTimer = new Timer(1.0, false, false, TimeToEndTimer);
    TimerManager::GetInstance().Add(TimeToEndTimer);

    std::random_device rd;

    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 4);

    randomNumber1 = dist(gen);


}

void Player::Draw() const
{

    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(position.x + sin(Otherrotation * 0.015) * 200, position.y + cos(Otherrotation * 0.015) * 200), size, -Otherrotation, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(position.x + size.x/4 + sin(glm::radians(Otherrotation - 90.0f * 0)) * size.x * 1, position.y + size.y/4 + cos(glm::radians(Otherrotation - 90.0f * 0))* size.y * 1), glm::vec2(size.x / 2, size.y / 2 ), -Otherrotation - 90.0f * 0, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(position.x + size.x/4 + sin(glm::radians(Otherrotation - 90.0f * 1)) * size.x * 1, position.y + size.y/4 + cos(glm::radians(Otherrotation - 90.0f * 1))* size.y * 1), glm::vec2(size.x / 2, size.y / 2 ), -Otherrotation - 90.0f * 1, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(position.x + size.x/4 + sin(glm::radians(Otherrotation - 90.0f * 2)) * size.x * 1, position.y + size.y/4 + cos(glm::radians(Otherrotation - 90.0f * 2))* size.y * 1), glm::vec2(size.x / 2, size.y / 2 ), -Otherrotation - 90.0f * 2, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(position.x + size.x/4 + sin(glm::radians(Otherrotation - 90.0f * 3)) * size.x * 1, position.y + size.y/4 + cos(glm::radians(Otherrotation - 90.0f * 3))* size.y * 1), glm::vec2(size.x / 2, size.y / 2 ), -Otherrotation - 90.0f * 3, MainColor, 1.0f);

    //std::vector<std::pair<std::string, std::string>> shadertexture = {{"image", "gd"}};
    // ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("quad", "sprite_sheet", shadertexture, 3, 1, position, size, rotation, MainColor, 1.0f);

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 3, randomNumber1, position, size, rotation, mainColor, 1.0f);

    /*
    ResourceManager::GetInstance().RenderLine(
        glm::vec3(position, 0.0f),
        glm::vec3(position + size, 0.0f),
        glm::vec3(position.x + size.x / 2, position.y + size.y / 2, 0.0f),
        glm::vec3(0.0f, 0.0f, rotation),
        glm::vec3(0.0f, 0.0f, 1.0f),
        1.0f,
        10.0f);

    ResourceManager::GetInstance().RenderTriangle(
        glm::vec3(position.x, position.y + size.y, 0.0f), 
        glm::vec3(position.x + size.x / 2, position.y, 0.0f), 
        glm::vec3(position + size, 0.0f), 
        glm::vec3(position.x + size.x / 2, position.y + size.y / 2, 0.0f), 
        glm::vec3(0.0f, 0.0f, rotation), 
        glm::vec3(1.0f, 0.5f, 0.5f), 
        0.1f, 
        5.0f);

    ResourceManager::GetInstance().RenderRectangle(
        glm::vec3(position, 0.0f), 
        glm::vec3(position + size, 0.0f), 
        glm::vec3(position.x + size.x / 2, position.y + size.y / 2, 0.0f), 
        glm::vec3(0.0f, 0.0f, rotation), 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        1.0f, 
        1.0f);
    */
}

void Player::Update(float deltaTime)
{


    bool KeyRightRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT, KeyEventType::KeyRelease);
    bool KeyLeftRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT, KeyEventType::KeyRelease);
    bool KeyUpRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP, KeyEventType::KeyRelease);
    bool KeyDownRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN, KeyEventType::KeyRelease);

    bool KeySpaceDown = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_SPACE, KeyEventType::KeyDown);
    bool KeySpaceRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_SPACE, KeyEventType::KeyRelease);

    bool RigthClickDown = InputManager::GetInstance().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyEventType::KeyDown);
    bool RigthClickRelease = InputManager::GetInstance().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyEventType::KeyRelease);


    bool MainJumpControlDown = (KeySpaceDown || RigthClickDown), MainJumpControlRelease = (KeySpaceRelease || RigthClickRelease);


    if(MainJumpControlDown)
    {
        ReleaseJumping = true;
    }

    if(!MainJumpControlRelease)
    {
        ReleaseJumping = false;
    }
    

    if(MainJumpControlRelease && OnGroundOrBlock)
    {
        velocity.y = -2000.0f;
        ReleaseJumping = false;
    }

    if(!OnGroundOrBlock)
    {
        angularVelocity = 400.0f;
    }
    else
    {
        auto currentRot = glm::mod(rotation, 90.0f);
        if(5 < currentRot && currentRot < 85)
        {
            if(currentRot >= 45)
            {
                angularVelocity = 500.0f;
            }
            else
            {
                angularVelocity = -500.0f;
            }
        }
        else
        {
            angularVelocity = 0;
            if(currentRot >= 45)
            {
                rotation = rotation - currentRot + 90;
            }
            else
            {
                rotation = rotation - currentRot;
            }

        }

    }


    
    if(velocity.y >= 0 && position.y + size.y > 0.0f)
    {
        OnGroundOrBlock = true;
        position.y = 0.0f - size.y;
        velocity.y = 0.0f;
    }
    else
    {
        OnGroundOrBlock = false;
    }



}

void Player::SetUniqueName(const std::string &name)
{
    this->UniqueName = name;
}

const std::string&  Player::GetUniqueName() const
{
    return this->UniqueName;
}

void Player::Body1CollisionEvent(Object *Other, BodyCollisionSide Side)
{

    IF_GET_TYPE(current, SolidBlock, Other)
    {
        switch (Side) {
            case BodyCollisionSide::TOP:
                if(CurrentPlayerMode == PlayerMode::Ship)
                {
                    if(velocity.y <= 0 && position.y + 4 > current->GetPosition().y + current->GetSize().y)
                    {
                        OnGroundOrBlock = true;
                        position.y = current->GetPosition().y + current->GetSize().y;
                        velocity.y = 0;  
                
                    }
                }
                break;

            case BodyCollisionSide::BOTTOM:
                if(CurrentPlayerMode == PlayerMode::Normal)
                {
                    if((velocity.y >= 0 && position.y + size.y < current->GetPosition().y + current->GetSize().y * 0.35))
                    {
                        OnGroundOrBlock = true;
                        position.y = current->GetPosition().y - size.y;
                        velocity.y = 0;  
                    
                    }
                }
                if(CurrentPlayerMode == PlayerMode::Ship)
                {
                    if(velocity.y >= 0 && position.y + size.y < current->GetPosition().y)
                    {
                        OnGroundOrBlock = true;
                        position.y = current->GetPosition().y - size.y;
                        velocity.y = 0;  
                    
                    }
                }
                break;

            default:
                break;
        }
    }

    
    IF_GET_TYPE(current, Orb, Other)
    {
        if(!current->IsUsed() && ReleaseJumping)
        {
            ReleaseJumping = false;

            OnGroundOrBlock = false;
            current->SetUsed();

            switch (current->GetOrbType())
            {
            case OrbType::Green:
                velocity.y = -2000;
                break;

            case OrbType::Blue:
                velocity.y = -1500;
                break;

            default:
                break;
            }
        }
    }

    IF_GET_TYPE(current, Spike, Other)
    {
        this->Destroy();
        SoundManager::GetInstance().Play("Dead", 0.5f, false);
    }

}

void Player::Body2CollisionEvent(Object *Other, BodyCollisionSide Side)
{
    IF_GET_TYPE(current, SolidBlock, Other)
    {
        this->Destroy();
        SoundManager::GetInstance().Play("Dead", 0.5f, false);
    }


}

Player::~Player()
{
    TimeToEndTimer->Destroy();
}
