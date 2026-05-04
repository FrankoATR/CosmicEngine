#include "GameInScene.h"
#include "MainScene.h"
#include "../Entities/MapTileObject.h"

GameInScene::GameInScene(GameManager* Game) : GameScene(Game)
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

    Game->resourceManager->loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
    Game->resourceManager->loadSpriteSheet("SpriteSheet", DUNGUEON1_IMAGE_PATH, 4, 4);
    Game->resourceManager->loadFont("Font", RETRO_FONT_PATH, 50);


    this->player = new LinkObject(Game, DynamicEntity, Vec2(100, 100), Vec2(64, 64), "NAME", Game->resourceManager->getBitmapRegionFromSpriteSheet("Player", 1, 1), 1, 20, Game->resourceManager->getFont("Font"));


    float load = 1.0;
    for(int i=0; i < 50; i++)
        for(int j=0; j < 30; j++){
            new MapTileObject(Game, DynamicEntity, Vec2(32*i, 32*j), Vec2(32, 32), "Tile", Game->resourceManager->getBitmapRegionFromSpriteSheet("SpriteSheet", rand()%2, rand()%4), 0);
            load++;
            SetProgressLoadingScene((load/(50.0f*30.0f)));
        }

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
        Game->gameSceneManager->PopScene();
    }
    if(Game->keyState[ALLEGRO_KEY_SPACE]){
        Game->gameSceneManager->PushScene(new MainScene(Game));
    }

}



GameInScene::~GameInScene()
{


}