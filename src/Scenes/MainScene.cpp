#include "MainScene.hpp"
#include <WandEngine/Managers/ResourceManager.hpp>
#include <WandEngine/Managers/SceneManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/CameraManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>
#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/MusicManager.hpp>
#include <WandEngine/Managers/SoundManager.hpp>
#include <WandEngine/Managers/UIManager.hpp>
#include <WandEngine/Collisions/GameGridCollisions.hpp>

#include "../Utilities/Paths.hpp"
#include "../Entities/Ground.hpp"
#include "../Entities/Background.hpp"
#include "../Entities/SolidBlock.hpp"
#include "../Entities/Spike.hpp"
#include "../Entities/Orb.hpp"
#include "../Entities/EndLevel.hpp"



MainScene::MainScene(int Level, int Attempts) : GameScene("MainScene")
{
    this->Current_Level = (Level % 3);
    this->Current_Attempts = Attempts;
}

void MainScene::Init()
{
    AddMainThreadTask([this]()
    {
        ResourceManager::GetInstance().loadFont("ThaleahFat", FONT_1_PATH, 75);

        ResourceManager::GetInstance().loadBitmap("BG1", BG_1_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("GS1", GS_1_IMAGE_PATH);

        ResourceManager::GetInstance().loadBitmap("BG2", BG_2_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("GS2", GS_2_IMAGE_PATH);

        ResourceManager::GetInstance().loadBitmap("BG3", BG_3_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("GS3", GS_3_IMAGE_PATH);


        ResourceManager::GetInstance().loadSpriteSheet("MISC", MISC_1_IMAGE_PATH, 6, 6);

        MusicManager::GetInstance().Load("Practice", TRACK0_PATH);
        MusicManager::GetInstance().Load("Electrodynamix", TRACK1_PATH);
        MusicManager::GetInstance().Load("Cycles", TRACK2_PATH);

        SoundManager::GetInstance().Load("Dead", SOUND1_PATH);
        SoundManager::GetInstance().Load("EndLevel", SOUND2_PATH);

        LoadMap();

        Respawn_Timer = new GameTimer(1.0, true, true);
        TimerManager::GetInstance().Add(Respawn_Timer);

        Attempts_Label = new UIText("Attempt", ResourceManager::GetInstance().getFont("ThaleahFat"), WAND_VEC2(100, 50), WAND_SIZE(300, 100), true, nullptr);
        UIManager::GetInstance().AddElement(Attempts_Label);

        //ToogleShowGrid();
        //ToogleShowCamera();
        BodyManager::GetInstance().SetNewGridArea(new GameGridCollisions(WAND_VEC2(-1000, -650 * 15), 20, 52, 650));


        SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(100.0f, 100.0f, 100.0f));
        SetProgressLoadingScene(1.0);
    });
}

void MainScene::Update(double deltaTime)
{
    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    GameObject* end = ObjectManager::GetInstance().FindByUniqueName("EndLevel");
    if(player)
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(player->GetPosition().x + 300, player->GetPosition().y - 150));
    }
    else
    {
        if(Respawn_Timer->IsPause())
        {
            if(!end)
            {
                Respawn_Timer->SetWaitTime(4.0);
            }
            else
            {
                SoundManager::GetInstance().Play("Dead", 1.0, false);
            }

            Respawn_Timer->Play();
            Respawn_Timer->Reset();
            MusicManager::GetInstance().Stop();
        }

        if(Respawn_Timer->IsTrigger() && !Respawn_Timer->IsPause())
        {
            if(!end)
            {
                Current_Level++;
                Current_Attempts = 0;
            }

            SceneManager::GetInstance().ReplaceScene(new MainScene(Current_Level, Current_Attempts+1));

        }
    }

    if(Attempts_Label)
    {
        if(Current_Level == 2 && !end)
        {
            Attempts_Label->SetText("GAME OVER - GAME COMPLETED");
            Attempts_Label->SetPosition(WAND_VEC2(600,500));
            Attempts_Label->SetTextColor(WAND_COLOR(255,0,200,255));
        }
        else
        {
            Attempts_Label->SetText("Attempt " + std::to_string(Current_Attempts));
        }
    }

    if(InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_R, KeyEventType::KeyDown))
    {
        SceneManager::GetInstance().ReplaceScene(new MainScene(Current_Level, Current_Attempts+1));
    }

    if(InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_H, KeyEventType::KeyDown))
    {
        ToogleShowBodys();
    }

    if(InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, KeyEventType::KeyDown))
    {
        SceneManager::GetInstance().PopScene();
    }

}



void MainScene::LoadMap()
{

    int StandarSizeEntities = 100;

    int map0[10][150] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,2,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,2},
        {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,2,1,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,2,2,2,1,2,2,1,0,0,0,0,0,2,2,0,0,0,0,0,0,0,1,2,2,2,2,2,0,0,0,0, 0,0,0,0,1,2,2,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,1,2,2,2,1,2,1,2,2,2,1,0,0,0,0,2, 2,2,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,1,0,0,0,0,0,0,0,1,2,1,1,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}
    };

    int map1[10][150] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,2,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,-2,0},
        {0,0,0,0,0,0,0,0,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0, 0,0,1,1,1,1,1,0,0,0,2,0,2,1,0,0,0,0,0,0,0,2,2,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,2,2,2,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,2,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0},
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,1,0,1,1,0,0,0,0,0,1,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,0,0,0,1,2,1,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,2,1,0,0,0, 0,0,0,0,2,2,2,1,2,0,0,0,0,0,1,2,2,2,1,2,1,0,0,0,0,0,1,2,2,2,1,1,1,1,2,1,1,1,0,1,0,0,0,0,0,0,1,2,2,2}
    };


    int map2[10][150] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,3,0,2,0,0,2,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0, 0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,2,1,1,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,2,1,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0, 0,1,0,0,0,0,4,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,3,0, 1,1,1,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,0,0,0,0,0,0,-2},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,3,0,0,0,1,0,0,0,0,0,0,0,3,0,2,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,3,0,1,1,1,0,0,0,0,0,1,1,1,2,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,4,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,3,0,0,2,0,0,2,1,0,0,0,3,0,0,1,0,0,0,0,0,3,0,0,0,0,1,0,0,4,0, 0,1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,3,0,0,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,1,0,1,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,4,0,0,0,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,1,2,2,1,1,2,2,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,1,0,2,2,2, 2,1,0,0,0,0,1,0,0,0,2,2,2,2,0,4,0,0,1,0,0,1,0,0,0,0,0,0,2,2,2,0,0,0,0,0,1,0,0,2,2,2,0,0,1,1,0,0,0,0, 2,0,0,0,0,2,0,0,1,2,0,0,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2}
    };

    int map[10][150];

    WAND_COLOR p_c(255, 255, 255, 255);
    WAND_COLOR bg_c(255, 255, 255, 255);
    WAND_COLOR g_c(255, 255, 255, 255);
    WAND_COLOR spike_c(255, 255, 255, 255);
    WAND_COLOR block_c(255, 255, 255, 255);

    std::string bg_name = "BG1";
    std::string g_name = "GS1";

    if(Current_Level == 0)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map0[i][j];
            }
        }

        p_c = WAND_COLOR(255, 200, 255, 255);
        bg_c = WAND_COLOR(200, 20, 100, 255);
        g_c = WAND_COLOR(255, 30, 20, 255);
        spike_c = WAND_COLOR(0, 255, 0, 255);
        block_c = WAND_COLOR(0, 255, 0, 255);

        bg_name = "BG1";
        g_name = "GS1";

        MusicManager::GetInstance().Play("Electrodynamix", 0.4, false);
    }
    else if(Current_Level == 1)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map1[i][j];
            }
        }
        p_c = WAND_COLOR(20, 200, 255, 255);
        bg_c = WAND_COLOR(20, 20, 100, 255);
        g_c = WAND_COLOR(150, 30, 200, 255);
        spike_c = WAND_COLOR(255, 255, 255, 255);
        block_c = WAND_COLOR(255, 255, 255, 255);

        bg_name = "BG3";
        g_name = "GS2";

        MusicManager::GetInstance().Play("Cycles", 0.4, false);
    }
    else if(Current_Level == 2)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map2[i][j];
            }
        }

        p_c = WAND_COLOR(20, 0, 100, 255);
        bg_c = WAND_COLOR(20, 200, 100, 255);
        g_c = WAND_COLOR(255, 255, 200, 255);
        spike_c = WAND_COLOR(100, 255, 100, 255);
        block_c = WAND_COLOR(100, 255, 100, 255);

        bg_name = "BG2";
        g_name = "GS3";

        MusicManager::GetInstance().Play("Practice", 0.4, false);
    }


    for(short unsigned int i = 0; i < 40; i++)
    {
        Ground* g1 = new Ground(WAND_VEC2(-800 + 450 * i, 825), WAND_VEC2(450, 450), ResourceManager::GetInstance().getBitmap(g_name), -1);
        g1->SetColor(g_c);
        ObjectManager::GetInstance().Add(g1);
    }

    for(short unsigned int i = 0; i < 10; i++)
    {
        Background* bg1 = new Background(WAND_VEC2(-1000 + 1080 * i*2, -1080), WAND_VEC2(1080*2, 1080*2), ResourceManager::GetInstance().getBitmap(bg_name), -2);
        bg1->SetColor(bg_c);
        ObjectManager::GetInstance().Add(bg1);
    }

    for(short unsigned int i = 0; i < 10; i++)
    {
        for(short unsigned int j = 0; j < 150; j++)
        {
            if(map[i][j] == -1)
            {
                Player* player = new Player(PlayerMode::Normal, WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 3, rand()%5), 10);
                player->SetColor(p_c);
                ObjectManager::GetInstance().Add(player);
            }

            if(map[i][j] == -2)
            {
                EndLevel* end = new EndLevel(WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 0, 5), 10);
                ObjectManager::GetInstance().Add(end);
            }

            if(map[i][j] == 1)
            {
                SolidBlock* b1 = new SolidBlock(WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 2, 1), 2);
                b1->SetColor(block_c);
                ObjectManager::GetInstance().Add(b1);
            }

            if(map[i][j] == 2)
            {
                Spike* s1 = new Spike(WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 4, rand()%3), 2);
                s1->SetColor(spike_c);
                ObjectManager::GetInstance().Add(s1);
            }

            if(map[i][j] == 3)
            {
                Orb* o1 = new Orb(OrbType::Green, WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 5, 0), 2);
                ObjectManager::GetInstance().Add(o1);
            }
            if(map[i][j] == 4)
            {
                Orb* o2 = new Orb(OrbType::Blue, WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 5, 1), 2);
                ObjectManager::GetInstance().Add(o2);
            }
        }
    }
}

