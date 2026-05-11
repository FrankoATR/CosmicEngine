#include "MainScene.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/MusicManager.hpp"
#include "../WandAllegroEngine/Managers/GameManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/CameraManager.hpp"

#include "GameInScene.hpp"
#include "../Entities/BackgroundObject.hpp"
#include "../Utilities/Paths.hpp"
#include "../Entities/LinkObject.hpp"
#include "../Entities/CustomEnemy.hpp"
#include "../Events/Logger.hpp"


MainScene::MainScene() : GameScene("MainScene")
{
}

void MainScene::Init()
{
    AddMainThreadTask([this]()
                      {
        ResourceManager::GetInstance().loadBitmap("Background1", BG_FOREST_IMAGE_PATH);
        ResourceManager::GetInstance().loadBitmap("Background2", BG_SPACE_IMAGE_PATH);
        ResourceManager::GetInstance().loadSpriteSheet("Mario Sprites", MARIO_SPRITES_PATH, 4, 4);
        ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
        
        ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50); });

    SetProgressLoadingScene(0.2f);

    AddMainThreadTask([this]()
                      {
        GameObject *bg = new BackgroundObject(Object::StaticEntity, Vec2(0, 0), Vec2(1920, 1080), "BG", ResourceManager::GetInstance().getBitmap("Background2"), 0);
        ObjectManager::GetInstance().Add(bg);

        GameObject *player = new LinkObject(Object::DynamicEntity, Vec2(400, 300), Vec2(64, 64), "Player", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Mario Sprites", 0, 0), 1, 20, ResourceManager::GetInstance().getFont("Font"));
        ObjectManager::GetInstance().Add(player);

        GameObject *enemy = new CustomEnemy(Object::DynamicEntity, Vec2(600, 600), Vec2(64, 64), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Mario Sprites", 0, 2), 1, 20, ResourceManager::GetInstance().getFont("Font"));
        ObjectManager::GetInstance().Add(enemy); });

    SetProgressLoadingScene(0.3f);

    AddMainThreadTask([this]()
                      {
        for (int i = 0; i < 10; i++)
        {
            GameObject *enemy = new CustomEnemy(Object::DynamicEntity, Vec2(rand()%(int)GameManager::GetInstance().GetWindowsSize().width, rand()%(int)GameManager::GetInstance().GetWindowsSize().height), Vec2(32, 32), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", rand()%2, rand()%5), 3, 20, ResourceManager::GetInstance().getFont("Font"));
            ObjectManager::GetInstance().Add(enemy);
        } });

    SetProgressLoadingScene(0.5f);

    // new LinkObject(DynamicEntity, Vec2(300, 200), Vec2(64, 64), "Enemy", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 3, 20, ResourceManager::GetInstance().getFont("Font"));

    EventManager::GetInstance().RegisterEvent("OnEnemyDestroy", std::function<void()>([]()
                                                                                      {
        GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
        if(player){
            player->SetPosition(Vec2(500, 500));
        }
        std::cout << "Enemigo destruido" << std::endl; }));

    EventManager::GetInstance().RegisterEvent(
        "ChangePosition",
        std::function<void(GameObject *, GameObject *)>([](GameObject *obj1, GameObject *obj2)
                                                        {
            Vec2 tmp = obj1->GetPosition();
            obj1->SetPosition(obj2->GetPosition());
            obj2->SetPosition(tmp);
            ALLEGRO_BITMAP* tmp2 = obj1->GetSprite();
            obj1->SetSprite(obj2->GetSprite());
            obj2->SetSprite(tmp2);
            std::string tmp3 = obj1->GetObjectName();
            obj1->SetObjectName(obj2->GetObjectName());
            obj2->SetObjectName(tmp3);
            std::cout << "Positions Changed" << std::endl; }));

    MusicManager::GetInstance().LoadMusic("Background", TRACK1_PATH);

    SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(155.0f, 0.0f, 33.0f, 0.0f));

    SetProgressLoadingScene(1.0f);

    MusicManager::GetInstance().PlayMusic("Background", 1.0f, true);

    std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl
              << std::endl;
}

void MainScene::Update(double deltaTime)
{

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, KeyDown))
    {
        SceneManager::GetInstance().PopScene();
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, KeyDown))
    {
        SceneManager::GetInstance().ReplaceScene(new GameInScene);
    }

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_H, KeyDown))
    {
        // GameManager::GetInstance().ToggleShowBody();
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
        GameManager::GetInstance().SetWindows_Size(Size(800, 640));
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_I, KeyDown))
    {
        GameManager::GetInstance().SetWindows_Size(Size(1920, 1080));
    }



    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_M, KeyDown))
    {
        GameObject *enemy = new CustomEnemy(Object::DynamicEntity, Vec2(rand()%(int)GameManager::GetInstance().GetWindowsSize().width, rand()%(int)GameManager::GetInstance().GetWindowsSize().height), Vec2(32, 32), "Emerson", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", rand()%2, rand()%5), 3, 20, ResourceManager::GetInstance().getFont("Font"));
        ObjectManager::GetInstance().Add(enemy);
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_N, KeyDown))
    {

    }


    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if (player)
    {
        CameraManager::GetInstance().FocusObject(player);
    }
    else{
        CameraManager::GetInstance().FocusPosition(Vec2(0.0f, 0.0f));
    }
}

MainScene::~MainScene()
{
}