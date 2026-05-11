#include "GameInScene.hpp"
#include "MainScene.hpp"
#include "../Entities/MapTileObject.hpp"
#include "../Entities/LinkObject.hpp"

#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"

GameInScene::GameInScene(GameManager* Game) : GameScene(Game, "GameInScene")
{

}

void GameInScene::UpdateLoadingScene(){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(50, Game->ScreenSize.Height / 2 - 10, 50 + 300 * GetProgressLoadingScene(), Game->ScreenSize.Height / 2 + 10, al_map_rgb(0, 255, 0));
    al_flip_display();
}


void GameInScene::Init()
{
    for (int key = ALLEGRO_KEY_A; key <= ALLEGRO_KEY_PAD_EQUALS; key++) {
        Game->keyState[key] = false;
    }


    WandEngine::ResourceManager::GetInstance().loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
    WandEngine::ResourceManager::GetInstance().loadSpriteSheet("SpriteSheet", DUNGUEON1_IMAGE_PATH, 4, 4);
    WandEngine::ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);


    GameObject* player = new LinkObject(Game, DynamicEntity, Vec2(100, 100), Vec2(64, 64), "Player", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Player", 1, 1), 1, 20, WandEngine::ResourceManager::GetInstance().getFont("Font"));
    WandEngine::ObjectManager::GetInstance().Add(player);

    /*
    
    float load = 1.0;
    for(int i=0; i < 10; i++)
        for(int j=0; j < 10; j++){
            GameObject* tmp = new MapTileObject(Game, DynamicEntity, Vec2(64*i, 64*j), Vec2(64, 64), "Tile", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
            WandEngine::ObjectManager::GetInstance().Add(tmp);
            load++;
            //SetProgressLoadingScene((load/(50.0f*30.0f)));
        }

    
    */

    for(int i = 0; i < 100; i++)
    {
            int x = rand()%1920;
            int y = rand()%1080; 
            GameObject* tmp = new MapTileObject(Game, DynamicEntity, Vec2(x, y), Vec2(64, 64), "Tile", WandEngine::ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
            WandEngine::ObjectManager::GetInstance().Add(tmp);
    }
    
    SetProgressLoadingScene(1.0f);

    Game->SetBackBufferColor(al_map_rgb(155, 141, 30));

}


void GameInScene::Update(double deltaTime)
{

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


    if (Game->keyState[ALLEGRO_KEY_ESCAPE]) {
        WandEngine::SceneManager::GetInstance().PopScene();
    }
    if(Game->keyState[ALLEGRO_KEY_H] && Game->Event.type == ALLEGRO_EVENT_KEY_DOWN){
        Game->ToggleShowBody();
    }
    if(Game->keyState[ALLEGRO_KEY_SPACE]){
        WandEngine::SceneManager::GetInstance().ReplaceScene(new MainScene(Game));
    }

}



GameInScene::~GameInScene()
{


}