#include "MainScene.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/MusicManager.hpp"
#include "../WandAllegroEngine/Managers/GameManager.hpp"

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
        WandEngine::ResourceManager::GetInstance().loadBitmap("Background1", BG_FOREST_IMAGE_PATH);
        WandEngine::ResourceManager::GetInstance().loadBitmap("Background2", BG_SPACE_IMAGE_PATH);
        WandEngine::ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
        WandEngine::ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
    });

    SetProgressLoadingScene(0.2f);


    AddMainThreadTask([this]()
    {
        GameObject *bg = new BackgroundObject(StaticEntity, Vec2(0, 0), Vec2(1920, 1080), "BG", WandEngine::ResourceManager::GetInstance().getBitmap("Background2"), 0);
        WandEngine::ObjectManager::GetInstance().Add(bg);

        GameObject *player = new LinkObject(DynamicEntity, Vec2(400, 300), Vec2(64, 64), "Player", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 4), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
        WandEngine::ObjectManager::GetInstance().Add(player);

        GameObject *enemy = new CustomEnemy(DynamicEntity, Vec2(600, 600), Vec2(64, 64), "Emerson", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 0, 2), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
        WandEngine::ObjectManager::GetInstance().Add(enemy);
    });

    SetProgressLoadingScene(0.3f);


    AddMainThreadTask([this]()
    {
        for (int i = 0; i < 5; i++)
        {
            GameObject *enemy = new CustomEnemy(DynamicEntity, Vec2(100*i, 200*i), Vec2(64, 64), "Emerson", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, i), 3, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
            WandEngine::ObjectManager::GetInstance().Add(enemy);
        
        }
    });

    SetProgressLoadingScene(0.5f);

    // new LinkObject(DynamicEntity, Vec2(300, 200), Vec2(64, 64), "Enemy", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 3, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));

    WandEngine::EventManager::GetInstance().RegisterEvent("OnEnemyDestroy", std::function<void()>([](){
        GameObject* player = WandEngine::ObjectManager::GetInstance().FindByUniqueName("Player");
        if(player){
            player->SetPosition(Vec2(500, 500));
        }
        std::cout << "Enemigo destruido" << std::endl;
    })
    );


    WandEngine::EventManager::GetInstance().RegisterEvent(
        "ChangePosition",
        std::function<void(GameObject*, GameObject*)>([](GameObject* obj1, GameObject* obj2){
            Vec2 tmp = obj1->GetPosition();
            obj1->SetPosition(obj2->GetPosition());
            obj2->SetPosition(tmp);
            ALLEGRO_BITMAP* tmp2 = obj1->GetSprite();
            obj1->SetSprite(obj2->GetSprite());
            obj2->SetSprite(tmp2);
            std::string tmp3 = obj1->GetObjectName();
            obj1->SetObjectName(obj2->GetObjectName());
            obj2->SetObjectName(tmp3);
            std::cout << "Positions Changed" << std::endl;
        })
    );


    WandEngine::MusicManager::GetInstance().LoadMusic("Background", "assets/music/Ludum Dare 38 - Track 5.wav");


    WandEngine::SceneManager::GetInstance().SetBackBufferColor(WandEngine::WAND_COLOR(155.0f, 0.0f, 33.0f, 0.0f));


    SetProgressLoadingScene(1.0f);

    WandEngine::MusicManager::GetInstance().PlayMusic("Background", 1.0f, true);

    std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl << std::endl;

}

void MainScene::Update(double deltaTime)
{

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, WandEngine::KeyDown))
    {
        WandEngine::SceneManager::GetInstance().PopScene();
    }

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, WandEngine::KeyDown))
    {
        WandEngine::SceneManager::GetInstance().ReplaceScene(new GameInScene);
    }
    

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_H, WandEngine::KeyDown))
    {
        //WandEngine::GameManager::GetInstance().ToggleShowBody();
    }

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_P, WandEngine::KeyDown))
    {
        WandEngine::GameManager::GetInstance().SetWindows_FullScreenMode();
    }
    else if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_O, WandEngine::KeyDown))
    {
        WandEngine::GameManager::GetInstance().SetWindows_WindowsMode();
    }

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_U, WandEngine::KeyDown))
    {
        WandEngine::GameManager::GetInstance().SetWindows_Size(Size(800, 640));
    }
    else if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_I, WandEngine::KeyDown))
    {
        WandEngine::GameManager::GetInstance().SetWindows_Size(Size(1920, 1080));
    }

    if(!WandEngine::ObjectManager::GetInstance().FindByUniqueName("Player")){
        WandEngine::SceneManager::GetInstance().ReplaceScene(new GameInScene);
    }


}

MainScene::~MainScene()
{
}