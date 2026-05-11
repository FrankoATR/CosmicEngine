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
#include "../WandAllegroEngine/Managers/CameraManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"


int destroyedTiles = 0;

GameInScene::GameInScene() : GameScene("GameInScene")
{

}

void GameInScene::Init()
{

    AddMainThreadTask([this]()
    {
        ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
        ResourceManager::GetInstance().loadSpriteSheet("SpriteSheet", DUNGUEON1_IMAGE_PATH, 4, 4);
        ResourceManager::GetInstance().loadSpriteSheet("Blocks Sprites", BLOCKS_SPRITES_PATH, 5, 3);
        ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
    });

    SetProgressLoadingScene(0.1f);


    AddMainThreadTask([this]()
    {
        GameObject* player = new LinkObject(Object::DynamicEntity, Vec2(100, 100), Vec2(64, 64), "Player", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 1, 20, ResourceManager::GetInstance().getFont("Font"));
        ObjectManager::GetInstance().Add(player);
    });

    SetProgressLoadingScene(0.2f);


    AddMainThreadTask([this]()
    {
        float load = 1.0;
        for(int i=0; i < 40; i++)
            for(int j=0; j < 20; j++){
                GameObject* tmp = new MapTileObject(Object::DynamicEntity, Vec2(100 + 32*i, 100 + 32*j), Vec2(32, 32), "Tile", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Blocks Sprites", rand()%5, rand()%3), 0);
                ObjectManager::GetInstance().Add(tmp);
                load++;
                SetProgressLoadingScene((load/(40.0f*20.0f)));
            }
    });


    /*
        for(int i = 0; i < 100; i++)
        {
                int x = rand()%1920;
                int y = rand()%1080; 
                GameObject* tmp = new MapTileObject(DynamicEntity, Vec2(x, y), Vec2(64, 64), "Tile", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
                ObjectManager::GetInstance().Add(tmp);
        }
    */
    
    destroyedTiles = 0;
    EventManager::GetInstance().RegisterEvent("OnTileDestroy", std::function<void()>([](){
            destroyedTiles++;
            std::cout << "Tiles Destroyed: " << destroyedTiles << std::endl;
    })
    );

    SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(155.0f, 141.0f, 30.0f, 0.0f));
    
    CameraManager::GetInstance().Reset();
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


    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_ESCAPE, KeyDown))
    {
        SceneManager::GetInstance().PopScene();
    }
    
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_SPACE, KeyDown))
    {
        SceneManager::GetInstance().ReplaceScene(new MainScene);
    }
    

    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_H, KeyDown))
    {
        //GameManager::GetInstance().ToggleShowBody();
    }

    if(destroyedTiles >= 50)
    {
        SceneManager::GetInstance().ReplaceScene(new MainScene);
    }

    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if (player)
    {
        CameraManager::GetInstance().FocusObject(player);
    }

}



GameInScene::~GameInScene()
{


}