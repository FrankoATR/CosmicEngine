#include "GameEditorScene.hpp"
#include "../Entities/MapTileObject.hpp"
#include "../Entities/LinkObject.hpp"

#include "../WandAllegroEngine/Managers/ResourceManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"
#include "../WandAllegroEngine/Managers/SceneManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/GameManager.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/CameraManager.hpp"
#include "../WandAllegroEngine/Managers/UIManager.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"

#include "MainScene.hpp"

#include "../WandAllegroEngine/Models/UIElements/UIButton.hpp"
#include "../Entities/BackgroundObject.hpp"

#include "../Utilities/Paths.hpp"

GameEditorScene::GameEditorScene() : GameScene("GameEditorScene"), CurrentMousePosition(InputManager::GetInstance().GetMousePosition()), 
    LastMousePosition(InputManager::GetInstance().GetMousePosition()),
    ObjectInHand(NULL), last_time(0)
{

}

void GameEditorScene::Init()
{
    AddMainThreadTask([this]()
    {
        ResourceManager::GetInstance().loadSpriteSheet("Pokemon Map", POKEMONTILES_SPRITES_PATH, 29, 8);
        ResourceManager::GetInstance().loadSpriteSheet("Blocks Sprites", BLOCKS_SPRITES_PATH, 5, 3);
        ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
        ResourceManager::GetInstance().loadFont("ButtonFont", RETRO_FONT_PATH, 30);
        ResourceManager::GetInstance().loadBitmap("Button1", BUTTON1_SPRITE_PATH);
        SetProgressLoadingScene(0.1f);

        for(int i=0; i<8; i++)
        {
            for(int j=0; j<29; j++)
            {
                UIButton* btn = new UIButton(ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Pokemon Map", j, i), "", NULL, WAND_VEC2(i*32, j*32), WAND_SIZE(32, 32), true, NULL);

                btn->SetOnClick([this, i, j](){
                    int auxId = 0;
                    if(ObjectInHand)
                    {
                        auxId = ObjectInHand->GetObjectId();
                        ObjectManager::GetInstance().Remove(auxId);
                    }
                    ObjectInHand = new BackgroundObject(Object::StaticEntity, WAND_VEC2(0, 0), WAND_VEC2(64, 64), "BG", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Pokemon Map", j, i), 0);
                    ObjectManager::GetInstance().Add(ObjectInHand);
                });

                UIManager::GetInstance().AddElement(btn);

            }

        }


        UIButton* button1 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "EXIT", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(900, 0), WAND_SIZE(150, 75), true, NULL);
        button1->SetOnClick([this](){
            SceneManager::GetInstance().ReplaceScene(new MainScene);
        });
        UIManager::GetInstance().AddElement(button1);
            

        SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(120.0f, 20.0f, 20.0f, 0.0f));
        
        CameraManager::GetInstance().Reset();
        SetProgressLoadingScene(1.0f);
        std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl << std::endl;

    });

}


void GameEditorScene::Update(double deltaTime)
{


    if(ObjectInHand)
    {
        WAND_VEC2 auxPos = InputManager::GetInstance().GetMousePosition();
        WAND_VEC2 auxSize = ObjectInHand->GetSize();
        int newPosX = (int)auxPos.x - (int)auxPos.x%(int)auxSize.x;
        int newPosY = (int)auxPos.y - (int)auxPos.y%(int)auxSize.y;
        ObjectInHand->SetPosition( WAND_VEC2(newPosX, newPosY) );

        if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyDown))
        {
            ObjectInHand = nullptr;
        }
    }

    if (InputManager::GetInstance().IsMouseButtonPressed(2, KeyDown))
    {
        CameraManager::GetInstance().FocusPosition(InputManager::GetInstance().GetMousePosition());
    }


    WAND_VEC2 actPos = CameraManager::GetInstance().GetFocusPosition();
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x-deltaTime*200, actPos.y));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x+deltaTime*200, actPos.y));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x, actPos.y-deltaTime*200));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x, actPos.y+deltaTime*200));
    }

}
