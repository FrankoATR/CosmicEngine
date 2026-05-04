#include "MainScene.h"
#include "GameInScene.h"
#include "../Entities/BackgroundObject.h"

MainScene::MainScene(GameManager* Game) : GameScene(Game)
{

}


void MainScene::UpdateLoadingScene(){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(50, Game->ScreenSize.Height / 2 - 10, 50 + 300 * GetProgressLoadingScene(), Game->ScreenSize.Height / 2 + 10, al_map_rgb(0, 255, 0));
    al_flip_display();
}


void MainScene::Init()
{

    for (int key = ALLEGRO_KEY_A; key <= ALLEGRO_KEY_PAD_EQUALS; key++) {
        Game->keyState[key] = false;
    }

    Game->resourceManager->loadBitmap("Background", BG_FOREST_IMAGE_PATH);
    Game->resourceManager->loadSpriteSheet("Player", CHARACTERS_IMAGE_PATH, 2, 7);
    Game->resourceManager->loadFont("Font", RETRO_FONT_PATH, 50);
    SetProgressLoadingScene(0.2f);


    //new BackgroundObject(Game, StaticEntity, Vec2(0, 0), Vec2(1920, 1080), "BG", Game->resourceManager->getBitmap("Background"), 0);
    SetProgressLoadingScene(0.4f);


    this->player = new LinkObject(Game, DynamicEntity, Vec2(400, 300), Vec2(64, 64), "PLAYER", Game->resourceManager->getBitmapRegionFromSpriteSheet("Player", 1, 4), 1, 20, Game->resourceManager->getFont("Font"));
    SetProgressLoadingScene(0.6f);

    for(int i=0; i < 5; i++){
        //new LinkObject(Game, DynamicEntity, Vec2(100*i, 200*i), Vec2(64, 64), "Enemy", Game->resourceManager->getBitmapRegionFromSpriteSheet("Player", 1, i), 3, 20, Game->resourceManager->getFont("Font"));
    }
    SetProgressLoadingScene(0.8f);

    //new LinkObject(Game, DynamicEntity, Vec2(300, 200), Vec2(64, 64), "Enemy", Game->resourceManager->getBitmapRegionFromSpriteSheet("Player", 1, 1), 3, 20, Game->resourceManager->getFont("Font"));
    SetProgressLoadingScene(1.0f);
        


}


void MainScene::Update(double deltaTime)
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
        Game->gameSceneManager->PushScene(new GameInScene(Game));
    }
    if(Game->keyState[ALLEGRO_KEY_H]){
        Game->ToggleShowBody();
    }

    if(Game->keyState[ALLEGRO_KEY_P]){
        Game->SetWindows_FullScreenMode();
    }
    else if(Game->keyState[ALLEGRO_KEY_O]){
        Game->SetWindows_WindowsMode();
    }

    if(Game->keyState[ALLEGRO_KEY_U]){
        Game->SetWindows_Size(Size(800, 640));
    }
    else if(Game->keyState[ALLEGRO_KEY_I]){
        Game->SetWindows_Size(Size(1920, 1080));
    }


}



MainScene::~MainScene()
{


}