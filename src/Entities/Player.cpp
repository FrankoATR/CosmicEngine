#include "Player.hpp"
#include "Orb.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>
#include <WandEngine/Managers/ResourceManager.hpp>

Player::Player(PlayerMode mode, WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::DynamicEntity, Position, Size, "Player", Sprite, LayerId)
{
    Velocity  = 400;
    OnGroundOrBlock = false;
    Direction.x = 2.4;
    KeySpaceDown = false;
    RigthClickDown = false;
    KeySpaceRelease = false;
    RigthClickRelease = false;
    ReleaseJumping  = false;
}


void Player::Init()
{

    Body1 = new GameBodyObject(this, Position, GetSize(), [this](GameObject* Other, CollisionSide Side){Body1CollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body1);


    WAND_VEC2 collisionSize = {GetSize().x * 0.3f, GetSize().y * 0.3f};
    WAND_VEC2 collisionPosition = {
        Position.x + (GetSize().x - collisionSize.x) / 2,
        Position.y + (GetSize().y - collisionSize.y) / 2
    };

    Body2 = new GameBodyObject(this, collisionPosition, collisionSize, [this](GameObject* Other, CollisionSide Side){Body2CollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body2);

    RotateSprite_Timer = new GameTimer(0.030, true, true);
    TimerManager::GetInstance().Add(RotateSprite_Timer);

    Update_Gravity_Timer = new GameTimer(0.005, true, false);
    TimerManager::GetInstance().Add(Update_Gravity_Timer);

}


void Player::Draw()
{
    if(Sprite){
        float cx = al_get_bitmap_width(Sprite) / 2.0f;
        float cy = al_get_bitmap_height(Sprite) / 2.0f;

        float adjusted_x = Position.x + (Size.x / 2.0f);
        float adjusted_y = Position.y + (Size.y / 2.0f);
        float rad = Rotation * ALLEGRO_PI / 180.0f; 
        al_draw_tinted_scaled_rotated_bitmap(
            Sprite,
            al_map_rgba(MainColor.r, MainColor.g, MainColor.b, MainColor.a), 
            cx, cy,
            adjusted_x, adjusted_y,
            Size.x / al_get_bitmap_width(Sprite), 
            Size.y / al_get_bitmap_height(Sprite), 
            rad, 
            0
        );    
    }
}


void Player::Update(float deltaTime)
{
    LastPosition = Position;

    KeySpaceRelease = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, KeyEventType::KeyRelease);
    RigthClickRelease = InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyRelease);
    KeySpaceDown = InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, KeyEventType::KeyDown);
    RigthClickDown = InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyDown);

    if(KeySpaceDown || RigthClickDown)
    {
        ReleaseJumping = true;
    }

    if(!KeySpaceRelease && !RigthClickRelease)
    {
        ReleaseJumping = false;
    }
    
    if( (KeySpaceRelease || RigthClickRelease) && OnGroundOrBlock)
    {
        Direction.y = -4.6;
        OnGroundOrBlock = false;
        ReleaseJumping = false;
    }

    if(OnGroundOrBlock)
    {
        if(!RotateSprite_Timer->IsPause())
        {
            if(Rotation % 90 >= 45)
            {
                Rotation = Rotation - Rotation % 90 + 90;
            }
            else
            {
                Rotation = Rotation - Rotation % 90;
            }

            RotateSprite_Timer->Pause();
            RotateSprite_Timer->Reset();
        }
    }
    else
    {
        if(RotateSprite_Timer->IsPause())
        {
            RotateSprite_Timer->Play();
        }
        else
        {
            if(RotateSprite_Timer->IsTrigger())
            {
                Rotation += 10;
            }
        }
    }


    Rotation = Rotation % 360;


    if(Update_Gravity_Timer->IsTrigger())
    {
        Direction.y += 0.1;
    }

    if(Direction.y > 6.0)
    {
        Direction.y = 6.0;
    }


    UpdatePosition(Velocity, deltaTime);
    Body1->SetPosition(Position);
    
    WAND_VEC2 collisionSize = {GetSize().x * 0.3f, GetSize().y * 0.3f};
    WAND_VEC2 collisionPosition = {
        Position.x + (GetSize().x - collisionSize.x) / 2,
        Position.y + (GetSize().y - collisionSize.y) / 2
    };
    Body2->SetPosition(collisionPosition);

    //Label_Info1->SetPosition(WAND_VEC2(100, 0));
    //Label_Info1->SetText(OnGroundOrBlock ? "NOT ON AIR" : "ON AIR");

    OnGroundOrBlock = false;

}



void Player::Body1CollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "Ground")
    {
        OnGroundOrBlock = true;
        Position.y = Other->GetPosition().y - Size.y;
        Direction.y = 0;
    }


    if (Other->GetObjectName() == "SolidBlock")
    {
        switch (Side) {
            case CollisionSide::BOTTOM:
                OnGroundOrBlock = true;
                Position.y = Other->GetPosition().y - Size.y;
                Direction.y = 0;
                break;

            default:
                break;
        }
    }



    if (Other->GetObjectName() == "Orb" && ReleaseJumping)
    {
        Orb* orb = static_cast<Orb*>(Other);

        if(orb && !orb->IsUsed())
        {
            ReleaseJumping = false;

            OnGroundOrBlock = false;
            orb->SetUsed();

            switch (orb->GetOrbType())
            {
            case OrbType::Green:
                Direction.y = -4.6;
                break;

            case OrbType::Blue:
                Direction.y = -3.4;
                break;

            default:
                break;
            }
        }
    }


    if(Other->GetObjectName() == "Spike")
    {
        this->Destroy();
    }

}


void Player::Body2CollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "SolidBlock")
    {
        this->Destroy();
    }

}




Player::~Player()
{

}
