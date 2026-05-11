#include "MainScene.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/MusicManager.hpp"
#include "../WandAllegroEngine/Managers/SoundManager.hpp"
#include "../WandAllegroEngine/Managers/GameManager.hpp"

#include "../WandAllegroEngine/Collisions/GameGridCollisions.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"

#include "../WandAllegroEngine/Managers/UIManager.hpp"
#include "../WandAllegroEngine/Managers/CameraManager.hpp"


#include "GameInScene.hpp"
#include "GameEditorScene.hpp"
#include "../Entities/BackgroundObject.hpp"
#include "../Utilities/Paths.hpp"
#include "../Entities/LinkObject.hpp"
#include "../Entities/MapTileObject.hpp"
#include "../Entities/CustomEnemy.hpp"
#include "../Events/Logger.hpp"

#include "../Utilities/DataManager.hpp"

MainScene::MainScene() : GameScene("MainScene"), CurrentMousePosition(InputManager::GetInstance().GetMousePosition()), LastMousePosition(InputManager::GetInstance().GetMousePosition())
{
    this->InitialSound = false;
    this->last_time = 0;
    this->timeAfterDeath = 0;
    this->DeathSound = false;
    this->Text1 = nullptr;
}

void MainScene::Init()
{
    AddMainThreadTask([this]()
                      {

        ResourceManager::GetInstance().loadBitmap("HandEditor", CURSOR_SPRITE_PATH);

        ResourceManager::GetInstance().loadBitmap("Background1", BG_FOREST_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("Background2", BG_SPACE_IMAGE_PATH);

        ResourceManager::GetInstance().loadBitmap("Pokemon-center", POKEMONBUILDING_SPRITES_PATH);

        ResourceManager::GetInstance().loadSpriteSheet("Mario Sprites", MARIO_SPRITES_PATH, 4, 4);

        ResourceManager::GetInstance().loadSpriteSheet("Dungueon Entities", CHARACTERS_IMAGE_PATH, 2, 7);
        ResourceManager::GetInstance().loadSpriteSheet("Dungueon Tiles", DUNGUEON_IMAGE_PATH, 10, 10);
        ResourceManager::GetInstance().loadSpriteSheet("Blocks Sprites", BLOCKS_SPRITES_PATH, 5, 3);
        
        ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
        ResourceManager::GetInstance().loadFont("ButtonFont", RETRO_FONT_PATH, 30);
        ResourceManager::GetInstance().loadBitmap("Button1", BUTTON1_SPRITE_PATH);
        
        SoundManager::GetInstance().Load("PacmanSound1", SOUND1_PATH);
        SoundManager::GetInstance().Load("Intro", SOUND2_PATH);
        SoundManager::GetInstance().Load("PacmanDeath", SOUND3_PATH);
        

    SetProgressLoadingScene(0.2f);

    
        GameObject *build = new BackgroundObject(Object::StaticEntity, WAND_VEC2(100, 0), WAND_VEC2(80*4, 70*4), "Pokemon-center", ResourceManager::GetInstance().getBitmap("Pokemon-center"), 0);
        ObjectManager::GetInstance().Add(build);

        //GameObject *bg = new BackgroundObject(Object::StaticEntity, WAND_VEC2(0, 0), WAND_VEC2(1920, 1080), "BG", ResourceManager::GetInstance().getBitmap("Background2"), 0);
        //ObjectManager::GetInstance().Add(bg);

        int posX = 100;
        int posY = 100;

        //DataManager::GetInstance().LoadData(posX, posY);

        GameObject *player = new LinkObject(Object::DynamicEntity, WAND_VEC2(posX, posY), WAND_VEC2(64, 64), "Player", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Dungueon Entities", 0, 0), 1, 20, ResourceManager::GetInstance().getFont("Font"));
        ObjectManager::GetInstance().Add(player);

        //GameObject *enemy = new CustomEnemy(Object::DynamicEntity, WAND_VEC2(600, 600), WAND_VEC2(64, 64), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Mario Sprites", 0, 2), 1, 20, ResourceManager::GetInstance().getFont("Font"));
        //ObjectManager::GetInstance().Add(enemy); 

    SetProgressLoadingScene(0.3f);


        for (int i = 0; i < 10; i++)
        {
            GameObject *enemy = new CustomEnemy(Object::DynamicEntity, WAND_VEC2(200 + i*100, 200), WAND_VEC2(64, 64), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Dungueon Entities", rand()%2, rand()%7), 1, 20, ResourceManager::GetInstance().getFont("Font"));
            ObjectManager::GetInstance().Add(enemy);

            //GameObject* tmp = new MapTileObject(Object::DynamicEntity, WAND_VEC2(rand()%(int)GameManager::GetInstance().GetWindowsSize().width, rand()%(int)GameManager::GetInstance().GetWindowsSize().height), WAND_VEC2(64, 64), "Tile", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Blocks Sprites", rand()%5, rand()%3), 3);
            //ObjectManager::GetInstance().Add(tmp);
        }


    SetProgressLoadingScene(0.5f);

    // new LinkObject(DynamicEntity, WAND_VEC2(300, 200), WAND_VEC2(64, 64), "Enemy", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 3, 20, ResourceManager::GetInstance().getFont("Font"));

        EventManager::GetInstance().RegisterEvent("OnEnemyDestroy", std::function<void()>([]()
                                                                                        {
            GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
            if(player){
                player->SetPosition(WAND_VEC2(500, 500));
            }
            std::cout << "Enemigo destruido" << std::endl; }));

        EventManager::GetInstance().RegisterEvent(
            "ChangePosition",
            std::function<void(GameObject *, GameObject *)>([](GameObject *obj1, GameObject *obj2)
                {
                    WAND_VEC2 tmp = obj1->GetPosition();
                    obj1->SetPosition(obj2->GetPosition());
                    obj2->SetPosition(tmp);
                    //ALLEGRO_BITMAP* tmp2 = obj1->GetSprite();
                    //obj1->SetSprite(obj2->GetSprite());
                    //obj2->SetSprite(tmp2);
                    //std::string tmp3 = obj1->GetObjectName();
                    //obj1->SetObjectName(obj2->GetObjectName());
                    //obj2->SetObjectName(tmp3);
                    std::cout << "Positions Changed" << std::endl; 
                }
            )
        );




        UIButton* button1 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Game Over", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(0, 0), WAND_SIZE(150, 75), true, NULL);

        button1->SetOnClick([](){
            SoundManager::GetInstance().Play("PacmanDeath", 1.0f, false);
        });

        UIManager::GetInstance().AddElement(button1);


        UIButton* button2 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Next Scene", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(200, 0), WAND_SIZE(150, 75), true, NULL);

        button2->SetOnClick([](){
            SceneManager::GetInstance().ReplaceScene(new GameInScene);
        });

        UIManager::GetInstance().AddElement(button2);


        UIButton* button3 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Toggle Bodys", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(400, 0), WAND_SIZE(150, 75), true, NULL);

        button3->SetOnClick([this](){
                ToogleShowBodys();
                ToogleShowGrid();
                ToogleShowCamera();
        });

        UIManager::GetInstance().AddElement(button3);


        UIButton* button4 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Save Game", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(0, 100), WAND_SIZE(150, 75), true, NULL);

        button4->SetOnClick([this](){
                GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
                if (player)
                {
                    DataManager::GetInstance().SaveData(*player);
                }
        });

        UIManager::GetInstance().AddElement(button4);


        UIButton* button5 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Load Game", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(0, 200), WAND_SIZE(150, 75), true, NULL);

        button5->SetOnClick([this](){
            
            WAND_VEC2 pos = DataManager::GetInstance().LoadData().back().GetPosition();
            GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
            if (player)
            {
                player->SetPosition(pos);
            }
        });

        UIManager::GetInstance().AddElement(button5);


        UIButton* button6 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "EDITOR", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(0, 300), WAND_SIZE(150, 75), true, NULL);

        button6->SetOnClick([this](){
            SceneManager::GetInstance().ReplaceScene(new GameEditorScene);
        });

        UIManager::GetInstance().AddElement(button6);


        Text1 = new UIText("Camera view at: ", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1500, 0), WAND_SIZE(150, 75), true, NULL);
        UIManager::GetInstance().AddElement(Text1);



    InputManager::GetInstance().SetMouseSprite(ResourceManager::GetInstance().getBitmap("HandEditor"));


    BodyManager::GetInstance().SetNewGridArea(new GameGridCollisions(WAND_VEC2(-200, -200), 20, 20, 100));
    SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(155.0f, 0.0f, 33.0f, 0.0f));

    SetProgressLoadingScene(1.0f);


    std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl << std::endl;

                  });
}

void MainScene::Update(double deltaTime)
{

    if(Text1)
    {
        int x = CameraManager::GetInstance().GetFocusPosition().x;
        int y = CameraManager::GetInstance().GetFocusPosition().y;
        Text1->SetText("Camera view at: " + std::to_string(x) + ", " + std::to_string(y));
    }



    if(!InitialSound)
    {
        SoundManager::GetInstance().Play("Intro", 1.0f, false);
        InitialSound = true;
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, KeyDown))
    {
        SceneManager::GetInstance().PopScene();
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, KeyDown))
    {
        SceneManager::GetInstance().ReplaceScene(new GameInScene);
    }


    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_P, KeyDown))
    {
        GameManager::GetInstance().SetWindows_FullScreenMode();
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_O, KeyDown))
    {
        GameManager::GetInstance().SetWindows_WindowsMode();
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_U, KeyDown))
    {
        GameManager::GetInstance().SetWindows_Size(WAND_SIZE(800, 640));
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_I, KeyDown))
    {
        GameManager::GetInstance().SetWindows_Size(WAND_SIZE(1920, 1080));
    }



    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_M, KeyDown))
    {

    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_N, KeyDown))
    {

    }

    if (!UIManager::GetInstance().IsMouseHoverAny()) //Si el mouse no esta sobre algun elemento UI, deja hacer la accion
    {
        if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyDown))
        {
            GameObject *enemy = new CustomEnemy(Object::DynamicEntity, InputManager::GetInstance().GetMousePosition(), WAND_VEC2(32, 32), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Dungueon Entities", rand()%2, rand()%7), 3, 20, ResourceManager::GetInstance().getFont("Font"));
            ObjectManager::GetInstance().Add(enemy);
            SoundManager::GetInstance().Play("PacmanSound1", 1.0f, false);
        }
        if(InputManager::GetInstance().IsMouseButtonPressed(2, KeyDown))
        {
            GameObject* tmp = new MapTileObject(Object::DynamicEntity, InputManager::GetInstance().GetMousePosition(), WAND_VEC2(64, 64), "Tile", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Blocks Sprites", rand()%5, rand()%3), 3);
            ObjectManager::GetInstance().Add(tmp);
        }
    }


    /*
    
    if (InputManager::GetInstance().IsMouseButtonPressed(2, KeyRelease))
    {
        double current_time = al_get_time();
        if (current_time - last_time >= 0.5)
        {
            last_time = current_time;

            CurrentMousePosition = InputManager::GetInstance().GetMousePosition();
            float offX = CameraManager::GetInstance().GetPosition().x + LastMousePosition.x - CurrentMousePosition.x;
            float offY = CameraManager::GetInstance().GetPosition().y + LastMousePosition.y - CurrentMousePosition.y;
            CameraManager::GetInstance().FocusPosition(WAND_VEC2(offX, offY));
            LastMousePosition = CurrentMousePosition;

        }

    }

    */

    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if (player)
    {
        CameraManager::GetInstance().FocusObject(player);
    }
    else
    {
        if(!DeathSound){
            double current_time = al_get_time();
            if (current_time - timeAfterDeath >= 1.5)
            {
                timeAfterDeath = current_time;
                DeathSound = true;
            }
            if(DeathSound)
            {
                SoundManager::GetInstance().Play("PacmanDeath", 1.0f, false);
            }
        }
        
    }
}

