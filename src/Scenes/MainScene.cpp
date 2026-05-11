#include "MainScene.hpp"
#include <WandEngine/Managers/ResourceManager.hpp>
#include <WandEngine/Managers/SceneManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/CameraManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>
#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/MusicManager.hpp>
#include <WandEngine/Managers/SoundManager.hpp>
#include <WandEngine/Collisions/GameGridCollisions.hpp>

#include "../Utilities/Paths.hpp"
#include "../Entities/Ground.hpp"
#include "../Entities/Background.hpp"
#include "../Entities/SolidBlock.hpp"
#include "../Entities/Spike.hpp"

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
    {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,2,1,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,2,2,2,1,2,2,1,0,0,0,0,0,2,2,0,0,0,0,0,0,0,1,2,2,2,2,2,0,0,0,0, 0,0,0,0,1,2,2,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,1,2,2,2,1,2,1,2,2,2,1,0,0,0,0,2, 2,2,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,1,0,0,0,0,0,0,0,1,2,1,1,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}
};

int map1[10][150] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,2,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0, 0,0,1,1,1,1,1,0,0,0,2,0,2,1,0,0,0,0,0,0,0,2,2,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,2,2,2,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,2,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,1,0,1,1,0,0,0,0,0,1,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,0,0,0,1,2,1,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,2,1,0,0,0, 0,0,0,0,2,2,2,1,2,0,0,0,0,0,1,2,2,2,1,2,1,0,0,0,0,0,1,2,2,2,1,1,1,1,2,1,1,1,0,1,0,0,0,0,0,0,1,2,2,2}
};


MainScene::MainScene() : GameScene("MainScene")
{

}

void MainScene::Init()
{
    AddMainThreadTask([this]()
    {
        ResourceManager::GetInstance().loadFont("ThaleahFat", FONT_1_PATH, 50);

        ResourceManager::GetInstance().loadBitmap("BG3", BG_3_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("GS2", GS_2_IMAGE_PATH);
        ResourceManager::GetInstance().loadSpriteSheet("MISC", MISC_1_IMAGE_PATH, 6, 6);
        MusicManager::GetInstance().Load("Practice", TRACK0_PATH);
        MusicManager::GetInstance().Load("Electrodynamix", TRACK1_PATH);
        MusicManager::GetInstance().Load("Cycles", TRACK2_PATH);
        SoundManager::GetInstance().Load("Dead", SOUND1_PATH);
        SoundManager::GetInstance().Load("LevelEnd", SOUND2_PATH);


        Player* player = new Player(PlayerMode::Normal, WAND_VEC2(200, 825 - StandarSizeEntities), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 3, rand()%5), 1);
        ObjectManager::GetInstance().Add(player);

        for(short unsigned int i = 0; i < 40; i++)
        {
            Ground* g1 = new Ground(WAND_VEC2(-800 + 450 * i, 825), WAND_VEC2(450, 450), ResourceManager::GetInstance().getBitmap("GS2"), -1);
            ObjectManager::GetInstance().Add(g1);
        }

        for(short unsigned int i = 0; i < 10; i++)
        {
            Background* bg1 = new Background(WAND_VEC2(-600 + 1080 * i*2, -1080), WAND_VEC2(1080*2, 1080*2), ResourceManager::GetInstance().getBitmap("BG3"), -2);
            ObjectManager::GetInstance().Add(bg1);
        }


        for(short unsigned int i = 0; i < 10; i++)
        {
            for(short unsigned int j = 0; j < 150; j++)
            {
                if(map1[i][j] == 1)
                {
                    SolidBlock* b1 = new SolidBlock(WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 2, 1), 2);
                    ObjectManager::GetInstance().Add(b1);
                }

                if(map1[i][j] == 2)
                {
                    Spike* b1 = new Spike(WAND_VEC2(StandarSizeEntities * j, 825 - StandarSizeEntities*10 + StandarSizeEntities*i ), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 4, 0), 2);
                    ObjectManager::GetInstance().Add(b1);
                }
            }
        }


        Respawn_Timer = new GameTimer(1.0, true, true);
        TimerManager::GetInstance().Add(Respawn_Timer);

        MusicManager::GetInstance().Play("Cycles", 1.0, false);

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
    if(player)
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(player->GetPosition().x + 300, player->GetPosition().y - 150));
    }
    else
    {
        if(Respawn_Timer->IsPause())
        {
            Respawn_Timer->Play();
            Respawn_Timer->Reset();
            MusicManager::GetInstance().Pause();
            SoundManager::GetInstance().Play("Dead", 1.0, false);
        }

        if(Respawn_Timer->IsTrigger() && !Respawn_Timer->IsPause())
        {
            Player* player = new Player(PlayerMode::Normal, WAND_VEC2(200, 825 - StandarSizeEntities), WAND_VEC2(StandarSizeEntities, StandarSizeEntities), ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("MISC", 3, rand()%5), 1);
            ObjectManager::GetInstance().Add(player);
            MusicManager::GetInstance().Restart();
            Respawn_Timer->Pause();

            std::vector<GameObject *> list_bg = ObjectManager::GetInstance().GetAll();
            for(auto &e : list_bg)
            {
                if(e->GetObjectName() == "Background")
                {
                    e->Destroy();
                }
            }

            for(short unsigned int i = 0; i < 10; i++)
            {
                Background* bg1 = new Background(WAND_VEC2(-600 + 1080 * i*2, -1080), WAND_VEC2(1080*2, 1080*2), ResourceManager::GetInstance().getBitmap("BG3"), -2);
                ObjectManager::GetInstance().Add(bg1);
            }

        }
    }

    if(InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_R, KeyEventType::KeyDown))
    {
        if(player)
        {
            player->Destroy();
        }
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

