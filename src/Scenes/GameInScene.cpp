#include "GameInScene.hpp"
#include "MainScene.hpp"
#include "../Entities/MapTileObject.hpp"
#include "../Entities/LinkObject.hpp"

#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/GameManager.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"


int destroyedTiles = 0;

GameInScene::GameInScene() : GameScene("GameInScene")
{

}

void GameInScene::Init()
{

    AddMainThreadTask([this]()
    {
        WandEngine::ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
        WandEngine::ResourceManager::GetInstance().loadSpriteSheet("SpriteSheet", DUNGUEON1_IMAGE_PATH, 4, 4);
        WandEngine::ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
    });

    SetProgressLoadingScene(0.1f);


    AddMainThreadTask([this]()
    {
        GameObject* player = new LinkObject(DynamicEntity, Vec2(100, 100), Vec2(64, 64), "Player", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
        WandEngine::ObjectManager::GetInstance().Add(player);
    });

    SetProgressLoadingScene(0.2f);


    AddMainThreadTask([this]()
    {
        float load = 1.0;
        for(int i=0; i < 50; i++)
            for(int j=0; j < 30; j++){
                GameObject* tmp = new MapTileObject(DynamicEntity, Vec2(32*i, 32*j), Vec2(32, 32), "Tile", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
                WandEngine::ObjectManager::GetInstance().Add(tmp);
                load++;
                SetProgressLoadingScene((load/(50.0f*30.0f)));
            }
    });


    /*
        for(int i = 0; i < 100; i++)
        {
                int x = rand()%1920;
                int y = rand()%1080; 
                GameObject* tmp = new MapTileObject(DynamicEntity, Vec2(x, y), Vec2(64, 64), "Tile", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
                WandEngine::ObjectManager::GetInstance().Add(tmp);
        }
    */
    

    WandEngine::SceneManager::GetInstance().SetBackBufferColor(al_map_rgb(155, 141, 30));

    SetProgressLoadingScene(1.0f);
    
    std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl << std::endl;

}


void GameInScene::Update(double deltaTime)
{

    /*
        switch (Game->Event.type)
    {
        case ALLEGRO_EVENT_KEY_DOWN:
            Game->keyState[Game->Event.keyboard.keycode] = true;
            break;

        case ALLEGRO_EVENT_KEY_UP:
            Game->keyState[Game->Event.keyboard.keycode] = false;
            break;
        

        default:
            break;
    }

    */


    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, WandEngine::KeyDown))
    {
        WandEngine::SceneManager::GetInstance().PopScene();
    }
    
    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, WandEngine::KeyDown))
    {
        WandEngine::SceneManager::GetInstance().ReplaceScene(new MainScene);
    }
    

    if (WandEngine::InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_H, WandEngine::KeyDown))
    {
        //WandEngine::GameManager::GetInstance().ToggleShowBody();
    }

    if(destroyedTiles >= 25)
    {
        WandEngine::SceneManager::GetInstance().ReplaceScene(new MainScene);
    }

}



GameInScene::~GameInScene()
{


}