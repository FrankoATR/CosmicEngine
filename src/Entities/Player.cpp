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

Player::Player(std::string UniqueName, PlayerMode mode, glm::vec2 Position, glm::vec2 Size, float Rotation, short int LayerId) : 
    GameObject("Player", Position, Size, Rotation, LayerId), CurrentPlayerMode(mode)
{
    Velocity = glm::vec2(400.0f, 400.f);
    OnGroundOrBlock = false;
    Velocity.x = 950.0f;
    Velocity.y = 0.0f;
    Acceleration.y = 7000.0f;
    AngularVelocity = 0.0f;
    KeySpaceDown = false;
    RigthClickDown = false;
    KeySpaceRelease = false;
    RigthClickRelease = false;
    ReleaseJumping = false;
    this->UniqueName = UniqueName;
    OtherRotation = 0.0f;
}

void Player::Init()
{
    Body1 = new GameBodyObject(this, glm::vec2(0.0f), GetSize(), [this](GameObject *Other, CollisionSide Side)
                               { Body1CollisionEvent(Other, Side); });
    BodyManager::GetInstance().Add(Body1);


    glm::vec2 collisionSize = {GetSize().x * 0.4f, GetSize().y * 0.4f};
    glm::vec2 collisionPosition = {
        (GetSize().x - collisionSize.x) / 2,
        (GetSize().y - collisionSize.y) / 2
    };
    Body2 = new GameBodyObject(this, collisionPosition, collisionSize, [this](GameObject *Other, CollisionSide Side)
                               { Body2CollisionEvent(Other, Side); });
    BodyManager::GetInstance().Add(Body2);


    OtherRotationTimer = new GameTimer(0.01, true, false);
    TimerManager::GetInstance().Add(OtherRotationTimer);

    TimeToEndTimer = new GameTimer(1.0, false, false, TimeToEndTimer);
    TimerManager::GetInstance().Add(TimeToEndTimer);

    std::random_device rd;

    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 4);

    randomNumber1 = dist(gen);
}

void Player::Draw()
{

    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(Position.x + sin(OtherRotation * 0.015) * 200, Position.y + cos(OtherRotation * 0.015) * 200), Size, -OtherRotation, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(Position.x + Size.x/4 + sin(glm::radians(OtherRotation - 90.0f * 0)) * Size.x * 1, Position.y + Size.y/4 + cos(glm::radians(OtherRotation - 90.0f * 0))* Size.y * 1), glm::vec2(Size.x / 2, Size.y / 2 ), -OtherRotation - 90.0f * 0, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(Position.x + Size.x/4 + sin(glm::radians(OtherRotation - 90.0f * 1)) * Size.x * 1, Position.y + Size.y/4 + cos(glm::radians(OtherRotation - 90.0f * 1))* Size.y * 1), glm::vec2(Size.x / 2, Size.y / 2 ), -OtherRotation - 90.0f * 1, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(Position.x + Size.x/4 + sin(glm::radians(OtherRotation - 90.0f * 2)) * Size.x * 1, Position.y + Size.y/4 + cos(glm::radians(OtherRotation - 90.0f * 2))* Size.y * 1), glm::vec2(Size.x / 2, Size.y / 2 ), -OtherRotation - 90.0f * 2, MainColor, 1.0f);
    //ResourceManager::GetInstance().Render2DSprite("t1", glm::vec2(Position.x + Size.x/4 + sin(glm::radians(OtherRotation - 90.0f * 3)) * Size.x * 1, Position.y + Size.y/4 + cos(glm::radians(OtherRotation - 90.0f * 3))* Size.y * 1), glm::vec2(Size.x / 2, Size.y / 2 ), -OtherRotation - 90.0f * 3, MainColor, 1.0f);

    //std::vector<std::pair<std::string, std::string>> shadertexture = {{"image", "gd"}};
    // ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("quad", "sprite_sheet", shadertexture, 3, 1, Position, Size, Rotation, MainColor, 1.0f);

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 3, randomNumber1, Position, Size, Rotation, MainColor, 1.0f);

    /*
    ResourceManager::GetInstance().RenderLine(
        glm::vec3(Position, 0.0f),
        glm::vec3(Position + Size, 0.0f),
        glm::vec3(Position.x + Size.x / 2, Position.y + Size.y / 2, 0.0f),
        glm::vec3(0.0f, 0.0f, Rotation),
        glm::vec3(0.0f, 0.0f, 1.0f),
        1.0f,
        10.0f);

    ResourceManager::GetInstance().RenderTriangle(
        glm::vec3(Position.x, Position.y + Size.y, 0.0f), 
        glm::vec3(Position.x + Size.x / 2, Position.y, 0.0f), 
        glm::vec3(Position + Size, 0.0f), 
        glm::vec3(Position.x + Size.x / 2, Position.y + Size.y / 2, 0.0f), 
        glm::vec3(0.0f, 0.0f, Rotation), 
        glm::vec3(1.0f, 0.5f, 0.5f), 
        0.1f, 
        5.0f);

    ResourceManager::GetInstance().RenderRectangle(
        glm::vec3(Position, 0.0f), 
        glm::vec3(Position + Size, 0.0f), 
        glm::vec3(Position.x + Size.x / 2, Position.y + Size.y / 2, 0.0f), 
        glm::vec3(0.0f, 0.0f, Rotation), 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        1.0f, 
        1.0f);
    */
}

void Player::Update(float deltaTime)
{
    /*
        KeySpaceRelease = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT, KeyEventType::KeyRelease);
        RigthClickRelease = InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyRelease);
        KeySpaceDown = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT, KeyEventType::KeyDown);
        RigthClickDown = InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyDown);

    */

    if(OtherRotationTimer->IsTrigger())
    {
       OtherRotation += 0.5;
    }

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
        Velocity.y = -1750.0f;
        ReleaseJumping = false;
    }

    if(!OnGroundOrBlock)
    {
        AngularVelocity = 300.0f;
    }
    else
    {
        auto currentRot = glm::mod(Rotation, 90.0f);
        if(5 < currentRot && currentRot < 85)
        {
            if(currentRot >= 45)
            {
                AngularVelocity = 500.0f;
            }
            else
            {
                AngularVelocity = -500.0f;
            }
        }
        else
        {
            AngularVelocity = 0;
            if(currentRot >= 45)
            {
                Rotation = Rotation - currentRot + 90;
            }
            else
            {
                Rotation = Rotation - currentRot;
            }

        }

    }


    
    if(Velocity.y >= 0 && Position.y + Size.y > 0.0f)
    {
        OnGroundOrBlock = true;
        Position.y = 0.0f - Size.y;
        Velocity.y = 0.0f;
    }
    else
    {
        OnGroundOrBlock = false;
    }



    //SET MAX VEL.Y


    /* CONTROLS
    if (KeyRightRelease)
    {
        Velocity.x = 400.0f;
    }
    else if (KeyLeftRelease)
    {
        Velocity.x = -400.0f;
    }
    else
    {
        Velocity.x = 0;
    }

    */


    if(TimeToEndTimer && TimeToEndTimer->IsTrigger())
    {
        //std::cout << Position.x << " " << Position.y << std::endl;
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

void Player::Body1CollisionEvent(GameObject *Other, CollisionSide Side)
{

    IF_GET_TYPE(current, SolidBlock, Other)
    {
        switch (Side) {
            case CollisionSide::TOP:
                if(CurrentPlayerMode == PlayerMode::Ship)
                {
                    if(Velocity.y <= 0 && Position.y + 4 > current->GetPosition().y + current->GetSize().y)
                    {
                        OnGroundOrBlock = true;
                        Position.y = current->GetPosition().y + current->GetSize().y;
                        Velocity.y = 0;  
                
                    }
                }
                break;

            case CollisionSide::BOTTOM:
                if(CurrentPlayerMode == PlayerMode::Normal)
                {
                    if((Velocity.y >= 0 && Position.y + Size.y < current->GetPosition().y + current->GetSize().y * 0.30) ||
                        (Position.x + Size.x > current->GetPosition().x && current->GetPosition().x + current->GetSize().x < Position.x))
                    {
                        OnGroundOrBlock = true;
                        Position.y = current->GetPosition().y - Size.y;
                        Velocity.y = 0;  
                    
                    }
                }
                if(CurrentPlayerMode == PlayerMode::Ship)
                {
                    if(Velocity.y >= 0 && Position.y + Size.y < current->GetPosition().y)
                    {
                        OnGroundOrBlock = true;
                        Position.y = current->GetPosition().y - Size.y;
                        Velocity.y = 0;  
                    
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
                Velocity.y = -1750;
                break;

            case OrbType::Blue:
                Velocity.y = -1200;
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

void Player::Body2CollisionEvent(GameObject *Other, CollisionSide Side)
{
    IF_GET_TYPE(current, SolidBlock, Other)
    {
        this->Destroy();
        SoundManager::GetInstance().Play("Dead", 0.5f, false);
    }


}

Player::~Player()
{
    OtherRotationTimer->Destroy();
    TimeToEndTimer->Destroy();
}
