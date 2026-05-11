#include "MainScene.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
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

void MainScene::UpdateLoadingScene()
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    // al_draw_filled_rectangle(50, WandEngine::GameManager::GetInstance().ScreenSize.Height / 2 - 10, 50 + 300 * GetProgressLoadingScene(), WandEngine::GameManager::GetInstance().ScreenSize.Height / 2 + 10, al_map_rgb(0, 255, 0));
    al_flip_display();
}

void MainScene::Init()
{

    WandEngine::ResourceManager::GetInstance().loadBitmap("Background1", BG_FOREST_IMAGE_PATH);
    WandEngine::ResourceManager::GetInstance().loadBitmap("Background2", BG_SPACE_IMAGE_PATH);
    WandEngine::ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
    WandEngine::ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
    SetProgressLoadingScene(0.2f);

    GameObject *bg = new BackgroundObject(StaticEntity, Vec2(0, 0), Vec2(1920, 1080), "BG", WandEngine::ResourceManager::GetInstance().getBitmap("Background2"), 0);
    WandEngine::ObjectManager::GetInstance().Add(bg);

    SetProgressLoadingScene(0.4f);

    GameObject *player = new LinkObject(DynamicEntity, Vec2(400, 300), Vec2(64, 64), "Player", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 4), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
    WandEngine::ObjectManager::GetInstance().Add(player);

    GameObject *enemy = new CustomEnemy(DynamicEntity, Vec2(600, 600), Vec2(64, 64), "Emerson", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 0, 2), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
    WandEngine::ObjectManager::GetInstance().Add(enemy);

    SetProgressLoadingScene(0.6f);

    for (int i = 0; i < 5; i++)
    {
        // new LinkObject(DynamicEntity, Vec2(100*i, 200*i), Vec2(64, 64), "Enemy", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, i), 3, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
    }
    SetProgressLoadingScene(0.8f);

    // new LinkObject(DynamicEntity, Vec2(300, 200), Vec2(64, 64), "Enemy", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 3, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));

    EventManager::GetInstance().Add(new Logger);

    SetProgressLoadingScene(1.0f);

    WandEngine::GameManager::GetInstance().SetBackBufferColor(al_map_rgb(155, 0, 30));

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
        WandEngine::GameManager::GetInstance().ToggleShowBody();
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


}

MainScene::~MainScene()
{
}