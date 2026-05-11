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
#include "../WandAllegroEngine/Collisions/GameGridCollisions.hpp"

#include "MainScene.hpp"

#include "../WandAllegroEngine/Models/UIElements/UIButton.hpp"
#include "../Entities/BackgroundObject.hpp"
#include "../Entities/MapTileObject.hpp"
#include "../Entities/HandEditorObject.hpp"

#include "../Utilities/Paths.hpp"

GameEditorScene::GameEditorScene() : GameScene("GameEditorScene"), CurrentMousePosition(InputManager::GetInstance().GetMousePosition()), 
    LastMousePosition(InputManager::GetInstance().GetMousePosition()),
    ObjectInHand(NULL), last_time(0), ActualMode(HandMode::Drag)
{

}

void GameEditorScene::Init()
{
    AddMainThreadTask([this]()
    {
        ResourceManager::GetInstance().loadBitmap("HandEditor", CURSOR_SPRITE_PATH);
        ResourceManager::GetInstance().loadSpriteSheet("Pokemon Map", POKEMONTILES_SPRITES_PATH, 24, 14);
        ResourceManager::GetInstance().loadSpriteSheet("Blocks Sprites", BLOCKS_SPRITES_PATH, 5, 3);
        ResourceManager::GetInstance().loadFont("Font", RETRO_FONT_PATH, 50);
        ResourceManager::GetInstance().loadFont("ButtonFont", RETRO_FONT_PATH, 30);
        ResourceManager::GetInstance().loadBitmap("Button1", BUTTON1_SPRITE_PATH);
        SetProgressLoadingScene(0.1f);

        for(int i=0; i<14; i++)
        {
            for(int j=0; j<24; j++)
            {
                UIButton* btn = new UIButton(ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Pokemon Map", j, i), "", NULL, WAND_VEC2(i*32, j*32), WAND_SIZE(32, 32), true, NULL);

                btn->SetOnClick([this, i, j](){
                    if(ActualMode == HandMode::Drag)
                    {
                        if(ObjectInHand)
                        {
                            ObjectInHand->Destroy();
                        }
                        ObjectInHand = new BackgroundObject(Object::StaticEntity, WAND_VEC2(0, 0), WAND_VEC2(64, 64), "BG", ResourceManager::GetInstance().getBitmapRegionFromSpriteSheet("Pokemon Map", j, i), 0);
                        ObjectManager::GetInstance().Add(ObjectInHand);
                    }
                });

                UIManager::GetInstance().AddElement(btn);

            }

        }


        UIButton* button1 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "EXIT", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(900, 0), WAND_SIZE(150, 75), true, NULL);
        button1->SetOnClick([this](){
            SceneManager::GetInstance().ReplaceScene(new MainScene);
        });
        UIManager::GetInstance().AddElement(button1);
            

       
        //GameObject* Hand = new HandEditorObject(Object::StaticEntity, WAND_VEC2(0, 0), WAND_VEC2(64, 64), "HandEditor", ResourceManager::GetInstance().getBitmap("HandEditor"), 255);
        //ObjectManager::GetInstance().Add(Hand);



        UIButton* button2 = new UIButton(ResourceManager::GetInstance().getBitmap("Button1"), "Toggle Bodys", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1200, 0), WAND_SIZE(150, 75), true, NULL);

        button2->SetOnClick([this](){
                ToogleShowBodys();
                ToogleShowGrid();
                ToogleShowCamera();
        });

        UIManager::GetInstance().AddElement(button2);



        SceneManager::GetInstance().SetBackBufferColor(WAND_COLOR(120.0f, 20.0f, 20.0f, 0.0f));
        
        InputManager::GetInstance().SetMouseSprite(ResourceManager::GetInstance().getBitmap("HandEditor"));


        BodyManager::GetInstance().SetNewGridArea(new GameGridCollisions(WAND_VEC2(0, 0), 40, 40, 64));


        Label_1 = new UIText("Camera view at: ", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1600, 0), WAND_SIZE(150, 75), true, NULL);
        UIManager::GetInstance().AddElement(Label_1);


        Label_2 = new UIText("Objects in scene: ", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1600, 50), WAND_SIZE(150, 75), true, NULL);
        UIManager::GetInstance().AddElement(Label_2);

        Label_3 = new UIText("Object in hand: ", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1600, 100), WAND_SIZE(150, 75), true, NULL);
        UIManager::GetInstance().AddElement(Label_3);

        Label_4 = new UIText("Mouse position: ", ResourceManager::GetInstance().getFont("ButtonFont"), WAND_VEC2(1600, 150), WAND_SIZE(150, 75), true, NULL);
        UIManager::GetInstance().AddElement(Label_4);

        CameraManager::GetInstance().Reset();
        SetProgressLoadingScene(1.0f);
        std::cout << "\n\nSCENE CREATED: " << GetName() << std::endl << std::endl;

    });

}


void GameEditorScene::Update(double deltaTime)
{

    /*
    GameObject* HandEditor = ObjectManager::GetInstance().FindByUniqueName("HandEditor");
    if (HandEditor)
    {
        HandEditor->SetPosition(InputManager::GetInstance().GetMousePosition());
    }
    
    */

    if(!UIManager::GetInstance().IsMouseHoverAny())
    {
        
        if(ActualMode == HandMode::Drag || ActualMode == HandMode::Move)
        {
            if(ObjectInHand)
            {
                WAND_VEC2 auxPos = InputManager::GetInstance().GetMousePosition();
                WAND_VEC2 auxSize = ObjectInHand->GetSize();
                int OffSetX = (int)auxPos.x%(int)auxSize.x;
                int OffSetY = (int)auxPos.y%(int)auxSize.y;

                (int)auxPos.x >= 0 ? OffSetX *= 1 : OffSetX += (int)auxSize.x ; 
                (int)auxPos.y >= 0 ? OffSetY *= 1 : OffSetY += (int)auxSize.y ; 

                int newPosX = (int)auxPos.x - OffSetX;
                int newPosY = (int)auxPos.y - OffSetY;
                ObjectInHand->SetPosition( WAND_VEC2(newPosX, newPosY) );

                if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyDown))
                {
                    std::vector<GameObject*> objects = ObjectManager::GetInstance().FindByMousePosition();
                    for(auto it = objects.begin(); it!= objects.end();)
                    {
                        if(*it == ObjectInHand)
                        {
                            objects.erase(it);
                        }
                        else
                        {
                            it++;
                        }
                    }
                    if(objects.empty())
                    {
                        GameObject* NewObj = ObjectInHand->Clone();
                        ObjectManager::GetInstance().Add(NewObj);
                    }
                }
                else if (InputManager::GetInstance().IsMouseButtonPressed(2, KeyRelease))
                {
                    std::vector<GameObject*> objects = ObjectManager::GetInstance().FindByMousePosition();
                    for(auto it = objects.begin(); it!= objects.end();)
                    {
                        if(*it == ObjectInHand)
                        {
                            objects.erase(it);
                        }
                        else
                        {
                            it++;
                        }
                    }
                    if(objects.empty())
                    {
                        GameObject* NewObj = ObjectInHand->Clone();
                        ObjectManager::GetInstance().Add(NewObj);
                    }
                }
            }
        }

        if(ActualMode == HandMode::Delete)
        {
            if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyDown))
            {
                std::vector<GameObject*> objects = ObjectManager::GetInstance().FindByMousePosition();
                if(!objects.empty())
                {
                    GameObject* tmp = objects.back();
                    if(tmp)
                    {
                        tmp->Destroy();
                    }   
                }

            }
            else if (InputManager::GetInstance().IsMouseButtonPressed(2, KeyRelease))
            {
                std::vector<GameObject*> objects = ObjectManager::GetInstance().FindByMousePosition();
                if(!objects.empty())
                {
                    GameObject* tmp = objects.back();
                    if(tmp)
                    {
                        tmp->Destroy();
                    }   
                }

            }
        }

        if(ActualMode == HandMode::Move)
        {
            if (InputManager::GetInstance().IsMouseButtonPressed(1, KeyDown))
            {
                std::vector<GameObject*> objects = ObjectManager::GetInstance().FindByMousePosition();
                if(!objects.empty())
                {
                    GameObject* tmp = objects.back();
                    if(tmp)
                    {
                        ObjectInHand = tmp;
                    }   
                }
            }
        }

    }


    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_1, KeyDown))
    {
        if(ObjectInHand)
        {
            ObjectInHand->Destroy();
            ObjectInHand = nullptr;
        }
        ActualMode = HandMode::Drag;
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_2, KeyDown))
    {
        if(ObjectInHand)
        {
            ObjectInHand->Destroy();
            ObjectInHand = nullptr;
        }
        ActualMode = HandMode::Delete;
    }
    else if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_3, KeyDown))
    {
        if(ObjectInHand)
        {
            ObjectInHand->Destroy();
            ObjectInHand = nullptr;
        }
        ActualMode = HandMode::Move;
    }



    if(Label_1)
    {
        int x = CameraManager::GetInstance().GetFocusPosition().x;
        int y = CameraManager::GetInstance().GetFocusPosition().y;
        Label_1->SetText("Camera view at: " + std::to_string(x) + ", " + std::to_string(y));
    }
    if(Label_2)
    {
        Label_2->SetText("Objects in scene: " + std::to_string(ObjectManager::GetInstance().GetAll().size()));
    }
    if(Label_3)
    {
        std::string aux = "None";
        if(ObjectInHand)
        {
            aux = ObjectInHand->GetObjectName();
        }
        Label_3->SetText("Object in hand: " + aux);
    }
    if(Label_4)
    {
        int rel_x = InputManager::GetInstance().GetMousePosition().x;
        int rel_y = InputManager::GetInstance().GetMousePosition().y;
        Label_4->SetText(std::to_string(rel_x) + ", " + std::to_string(rel_y));

        int abs_x = InputManager::GetInstance().GetAbsoluteMousePosition().x;
        int abs_y = InputManager::GetInstance().GetAbsoluteMousePosition().y;
        Label_4->SetPosition(WAND_VEC2(abs_x + 20, abs_y + 20));
    }



    WAND_VEC2 actPos = CameraManager::GetInstance().GetFocusPosition();
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_A, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x-deltaTime*400, actPos.y));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_D, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x+deltaTime*400, actPos.y));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_W, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x, actPos.y-deltaTime*400));
    }
    if (InputManager::GetInstance().IsKeyPressed(ALLEGRO_KEY_S, KeyRelease))
    {
        CameraManager::GetInstance().FocusPosition(WAND_VEC2(actPos.x, actPos.y+deltaTime*400));
    }

}
