#include "MainScene.hpp"

#include "../Entities/Player.hpp"
#include "../Entities/SolidBlock.hpp"
#include "../Entities/Spike.hpp"
#include "../Entities/Orb.hpp"


#include "../Utilities/Paths.hpp"

#include <WandEngine/Interfaces/Definitions.hpp>
#include <imgui/imgui.h>

#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include CAMERAMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include BODYMANAGER_HEADER
#include GAMEGRIDCOLLISIONS_HEADER
#include MUSICMANAGER_HEADER
#include SOUNDMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER
#include DATABASEMANAGER_HEADER



MainScene::MainScene(int Level, int Attempts) : GameScene("MainScene")
{
    CurrentLevel = Level;
    currentMusic = "Electrodynamix";
    player = nullptr;
    ostVolume = 0.0f;
    fpsSliderValue = 60;
    ticksSliderValue = 20;
    vsynEnable = false;
    focusObjID = -1;
    mouseInitialPosForArea = glm::vec2(0.0f);
    mouseFinalPosForArea = glm::vec2(0.0f);
    mouseKeyDownForArea = false;
    cameraVelocity = glm::vec2(0.0f, 0.0f);
    showPanel = true;
    EditorMode = false;
}


void MainScene::LoadResources()
{


}

void MainScene::Init()
{
    CAM_MN.SetCameraMode(CameraMode::CAMERA_2D);
    CAM_MN.SetFocusPosition(glm::vec2(-200.0f, -400.0f));

    //RS_MN.loadShader("sprite", SHADER_SPRITE_VS, SHADER_SPRITE_FS);
    //RS_MN.loadShader("sprite_sheet", SHADER_SPRITESHEET_VS, SHADER_SPRITESHEET_FS);

    RS_MN.loadTexture("bg1", TEXTURE_GD_BG1, false);
    RS_MN.loadTexture("g1", TEXTURE_GD_G1, true);
    RS_MN.loadTexture("g2", TEXTURE_GD_G2, true);
    RS_MN.loadTexture("line", TEXTURE_GD_LINE, true);
    RS_MN.loadTextureSheet("gd", TEXTURESHEET_GD, true, 6, 6, 0);

    UI_MN.AddElement(new UIButton("BUTTON", nullptr, glm::vec2(1500.0f, 20.0f), glm::vec2(50.0f), true, nullptr));

    BOD_MN.SetNewGridArea(new GameGridCollisions(glm::vec2(-1000, -10 * 100), 25, 200, 100));
    
    MSC_MN.Load("Electrodynamix", MUSIC_GD_ELECTRODYNAMIX);
    MSC_MN.Load("Cycles", MUSIC_GD_CYCLES);
    MSC_MN.Load("Practice", MUSIC_GD_PRACTICE);
    MSC_MN.Load("FireAura", MUSIC_IMPOSSIBLE_GAME);

    SND_MN.Load("Dead", MUSIC_GD_DEAD);


    Reset();

    //SetProgressLoadingScene(1.0);
};


void MainScene::Reset()
{

    if(player)
    {
        player->unRerence(&player);
    }
    
    auto objs = OBJ_MN.GetAll();
    for(auto &obj : objs)
    {
        obj->Destroy();
    }

    LoadMap();

    player = new Player("MAIN", PlayerMode::Normal, glm::vec2(-200.0f), glm::vec2(100.0f), 0.0f, 0);
    OBJ_MN.Add(player);
    player->makeReference(&player);
    
    MSC_MN.StopAll();
    MSC_MN.Play(currentMusic, ostVolume, false);

    std::cout << "SCENE RESETED" << std::endl;
}



void MainScene::Draw()
{
    
    for(int i = 0; i < 3; i++)
    {
        ResourceManager::GetInstance().Render2DSprite("bg1", 
            glm::vec2(
                CAM_MN.GetPosition().x - glm::mod(CAM_MN.GetPosition().x, 1920.0f / 0.05f) * 0.05 + i * 1920.0f, 
                CAM_MN.GetPosition().y - glm::mod(CAM_MN.GetPosition().y, 1920.0f / 0.01f) * 0.01 + CAM_MN.GetBaseWindowSize().y
            ), 
            glm::vec2(1920.0f, 1920.0f), 0.0f, glm::vec3(0.2f, 0.7f, 0.3f));
    }
    
    for(int i = 0; i < 6; i++)
    {
        ResourceManager::GetInstance().Render2DSprite("g1", glm::vec2(CAM_MN.GetPosition().x - glm::mod(CAM_MN.GetPosition().x, 430.0f) + i * 430.0f , 0.0f) , glm::vec2(430.0f, 430.0f), 0.0f, glm::vec3(1.0f, 0.3f, 0.3f));
    }
    
    ResourceManager::GetInstance().Render2DSprite("line", glm::vec2(CAM_MN.GetFocusPosition().x, 0.0f - 76.0f * 10 + 2.0f * 1) , glm::vec2(2.0f * 2, 76.0f * 20), 90.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    if(showPanel)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10));

        if (ImGui::Begin("Static Transparent Window", nullptr, 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("Press ESC to exit");
            ImGui::Text("Press F11 to switch window/fullscreen");
            ImGui::Text("Press F3 to show/hide panel");
            ImGui::Text("Press 1, 2, 3 or 4 to select level");
            ImGui::Text("Press R to reset");
            ImGui::Text("Press H to show/hide hitboxs");
            ImGui::SliderFloat("Volume", &ostVolume, 0.0f, 2.0f);
            ImGui::SliderInt("FPS", &fpsSliderValue, 15, 240);
            ImGui::Checkbox("VSync", &vsynEnable);
            ImGui::SliderInt("Ticks", &ticksSliderValue, 0, 200);
    
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }

/*
    RS_MN.RenderRectangle(
        glm::vec3(INP_MN.GetMousePosition() - glm::vec2(100.0f), 0.0f), 
        glm::vec3(INP_MN.GetMousePosition() + glm::vec2(100.0f), 0.0f)
        );

    if(mouseKeyDownForArea)
    {
        RS_MN.RenderRectangle(
            glm::vec3(mouseInitialPosForArea, 0.0f), 
            glm::vec3(INP_MN.GetMousePosition() , 0.0f)
            );
    }
*/

}

void MainScene::Update(double deltaTime)
{

    MSC_MN.SetVolume(currentMusic, ostVolume);
    GM_MN.SetGameTicks(ticksSliderValue);

    if(vsynEnable)
    {
        GM_MN.EnableVsync();
    }
    else
    {
        GM_MN.DisableVsync();
    }

    if(!GM_MN.IsVSyncEnable())
    {
        GM_MN.SetTargetFPS(fpsSliderValue);
    }


    if(EditorMode)
    {
        if(player)
        {
            player->Destroy();
        }


        glm::vec2 cameraPos = CAM_MN.GetFocusPosition();
        cameraVelocity.y = 0;
        cameraVelocity.x = 0;
        
        if(INP_MN.IsKeyPressed(GLFW_KEY_W, KeyRelease))
        {
            cameraVelocity.y = -1500.0f;
        }
        else if(INP_MN.IsKeyPressed(GLFW_KEY_S, KeyRelease))
        {
            cameraVelocity.y = 1500.0f;
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_A, KeyRelease))
        {
            cameraVelocity.x = -1500.0f;
        }
        else if(INP_MN.IsKeyPressed(GLFW_KEY_D, KeyRelease))
        {
            cameraVelocity.x = 1500.0f;
        }


        cameraPos.y += cameraVelocity.y * deltaTime;
        cameraPos.x += cameraVelocity.x * deltaTime;

        CAM_MN.SetFocusPosition(cameraPos);



        
        if(INP_MN.IsKeyPressed(GLFW_KEY_G, KeyDown))
        {
            DB_MN.OpenDatabaseForSaving(("lvl_" + std::to_string(CurrentLevel)));
            SolidBlock::SaveToDB();
            Spike::SaveToDB();
            DB_MN.CloseDatabase();

        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_L, KeyDown))
        {
            auto objs = OBJ_MN.GetAll();
            for(auto &obj : objs)
            {
                obj->Destroy();
            }

            DB_MN.OpenDatabaseForLoading(("lvl_" + std::to_string(CurrentLevel)));

            std::string sql = "SELECT name FROM sqlite_master WHERE type='table';";
        
            auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
                std::string tableName = argv[0];
        
                if (tableName == "SolidBlock") {
                    SolidBlock::LoadFrom();
                }
                else if (tableName == "Spike") {
                    Spike::LoadFrom();
                }

                return 0;
            };
        
            DB_MN.ExecuteQuery(sql, callback, nullptr);

            DB_MN.CloseDatabase();



        }

        glm::vec2 mousePos = INP_MN.GetMousePosition();
        glm::vec2 objSize(100.0f);
        glm::vec2 objPos(
            std::round((mousePos.x - objSize.x * 0.5) / 100.0f) * 100.0f,
            std::round((mousePos.y - objSize.y * 0.5) / 100.0f) * 100.0f
        );
        
        if(INP_MN.IsKeyPressed(GLFW_KEY_0, KeyDown))
        {
            if (selectedObject) selectedObject->Destroy();
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_1, KeyDown))
        {
            if (selectedObject) selectedObject->Destroy();
            selectedObject = new SolidBlock(0, glm::vec2(objPos), glm::vec2(objSize), -1);
            selectedObject->makeReference(&selectedObject);
            OBJ_MN.Add(selectedObject);
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_2, KeyDown))
        {
            if (selectedObject) selectedObject->Destroy();
            selectedObject = new Spike(static_cast<SpikeType>(0), glm::vec2(objPos), glm::vec2(objSize), -1);
            selectedObject->makeReference(&selectedObject);
            OBJ_MN.Add(selectedObject);
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_3, KeyDown))
        {
        
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_4, KeyDown))
        {
        
        }



        if(selectedObject)
        {
            selectedObject->SetPosition(objPos);
            if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyDown))
            {
                OBJ_MN.Add(selectedObject->Clone());
            }

            if(INP_MN.IsKeyPressed(GLFW_KEY_SPACE, KeyDown))
            {
                selectedObject->SetRotation(selectedObject->GetRotation() + 90.0f);
            }
        }

        if(!selectedObject)
        {
            if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyDown))
            {
                auto objs = OBJ_MN.FindByMousePosition();
                if(!objs.empty())
                {
                    objs.back()->makeReference(&selectedObject);
                }
            }
            if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2, KeyDown))
            {
                auto objs = OBJ_MN.FindByMousePosition();
                if(!objs.empty())
                {
                    objs.back()->Destroy();
                }
            }
        }

    }
    else
    {
        auto files = GM_MN.GetDroppedFiles();
        if(!files.empty())
        {
            std::cout << files.back() << std::endl;

            auto objs = OBJ_MN.GetAll();
            for(auto &obj : objs)
            {
                obj->Destroy();
            }

            DB_MN.OpenDatabaseForLoading(files.back());

            std::string sql = "SELECT name FROM sqlite_master WHERE type='table';";
        
            auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
                std::string tableName = argv[0];
        
                if (tableName == "SolidBlock") {
                    SolidBlock::LoadFrom();
                }
                else if (tableName == "Spike") {
                    Spike::LoadFrom();
                }

                return 0;
            };
        
            DB_MN.ExecuteQuery(sql, callback, nullptr);

            DB_MN.CloseDatabase();



        }

        if (selectedObject) selectedObject->Destroy();
        
        if(player)
        {

            glm::vec2 cameraPos = {player->GetPosition().x + 350.0f, CAM_MN.GetFocusPosition().y};
            
            if(player->GetPosition().y < CAM_MN.GetPosition().y + CAM_MN.GetBaseWindowSize().y * 0.3)
            {
                cameraVelocity.y = -1000.0f;
            }
            else if(player->GetPosition().y > CAM_MN.GetPosition().y + CAM_MN.GetBaseWindowSize().y * 0.7)
            {
                cameraVelocity.y = 1000.0f;
            }
            else
            {
                cameraVelocity.y = 0;
            }


            cameraPos.y += cameraVelocity.y * deltaTime;

            CAM_MN.SetFocusPosition(cameraPos);

            //CAM_MN.SetFocusObject(player, 350.0f, cameraPosY);
        }
        else
        {
            player = new Player("MAIN", PlayerMode::Normal, glm::vec2(-200.0f), glm::vec2(100.0f), 0.0f, 0);
            OBJ_MN.Add(player);
            player->makeReference(&player);
        }


        if(INP_MN.IsKeyPressed(GLFW_KEY_1, KeyDown))
        {
            CurrentLevel = 0;
            currentMusic = "Electrodynamix";
            Reset();
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_2, KeyDown))
        {
            CurrentLevel = 1;
            currentMusic = "Cycles";
            Reset();
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_3, KeyDown))
        {
            CurrentLevel = 2;
            currentMusic = "Practice";
            Reset();
        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_4, KeyDown))
        {
            CurrentLevel = 3;
            currentMusic = "FireAura";
            Reset();
        }

        if(INP_MN.IsKeyPressed(GLFW_KEY_R, KeyDown))
        {
            Reset();
        }
    
    }



    if(INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, KeyDown))
    {
        GM_MN.EndProgram();
    }



    if(INP_MN.IsKeyPressed(GLFW_KEY_F3, KeyDown))
    {
        showPanel = !showPanel;
    }
    if(INP_MN.IsKeyPressed(GLFW_KEY_H, KeyDown))
    {
        ToogleShowBodys();
    }




    if(INP_MN.IsKeyPressed(GLFW_KEY_E, KeyDown))
    {
        EditorMode = !EditorMode;
    }


    if(INP_MN.IsKeyPressed(GLFW_KEY_F11, KeyDown))
    {
        if(GM_MN.IsFullScreen())
            GM_MN.SetWindows_WindowsMode(960, 540);
        else
            GM_MN.SetWindows_FullScreenMode();
    }


    /*
    if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyDown))
    {
        mouseInitialPosForArea = INP_MN.GetMousePosition();
        mouseKeyDownForArea  = true;
    }
    if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyUp))
    {
        mouseFinalPosForArea = INP_MN.GetMousePosition();
        mouseKeyDownForArea  = false;

        auto objs = OBJ_MN.FindByArea(mouseInitialPosForArea, mouseFinalPosForArea);
        for(auto &obj : objs)
        {
            //obj->Destroy();
        }

    }

    */




/*
    if(InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT_CONTROL, KeyEventType::KeyDown))
        CameraManager::GetInstance().MovementSpeed = 5.0f;
    else
        CameraManager::GetInstance().MovementSpeed = 2.5f;
*/

}







void MainScene::LoadMap()
{


    //Respawn_Timer = new GameTimer(1.0, true, true);
    //TimerManager::GetInstance().Add(Respawn_Timer);

    //CameraMovement_Timer = new GameTimer(0.001, true, false);
    //TimerManager::GetInstance().Add(CameraMovement_Timer);

    //Attempts_Label = new UIText("Attempt", ResourceManager::GetInstance().getFont("ThaleahFat"), WAND_VEC2(100, 50), WAND_SIZE(300, 100), true, nullptr);
    //UIManager::GetInstance().AddElement(Attempts_Label);

    //ToogleShowGrid();
    //ToogleShowCamera();

    float StandarSizeEntities = 100.0f;
    int map_H = 10;
    int map_W = 150;

    int map0[map_H][map_W] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,2,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,2},
        {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,2,1,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,2,2,2,1,2,2,1,0,0,0,0,2,2,2,0,0,0,0,0,0,0,1,2,2,2,2,2,0,0,0,0, 0,0,0,0,1,2,2,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,1,2,2,2,1,2,1,2,2,2,1,2,2,0,0,2, 2,2,0,0,0,0,2,2,1,0,0,0,0,2,2,2,2,1,0,0,0,0,0,0,0,1,2,1,1,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}
    };

    int map1[map_H][map_W] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0, 0,0,0,0,0,2,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,-2,0},
        {0,0,0,0,0,0,0,0,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0, 0,0,1,1,1,1,1,0,0,0,2,0,2,1,0,0,0,0,0,0,0,2,2,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,2,2,2,0,0,0,1,1,0,0,4,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,4,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,2,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,2,2,2,2,1,1,0,1,1,0,0,0,0,0,1,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,0,0,0,1,2,1,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,2,1,0,0,0, 0,0,0,0,2,2,2,1,2,0,0,0,0,0,1,2,2,2,1,2,1,0,0,0,0,0,1,2,2,2,1,1,1,1,2,1,1,1,0,1,0,0,0,0,0,0,1,2,2,2}
    };


    int map2[map_H][map_W] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,3,0,2,0,0,2,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0, 0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,2,1,1,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,2,1,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0, 0,1,0,0,0,0,4,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,3,0, 1,1,1,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,0,0,0,0,0,0,-2},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,3,0,0,0,1,0,0,0,0,0,0,0,3,0,2,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,3,0,1,1,1,0,0,0,0,0,1,1,1,2,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,4,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,3,0,0,2,0,0,2,1,0,0,0,3,0,0,1,0,0,0,0,0,3,0,0,0,0,1,0,0,4,0, 0,1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,3,0,0,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,1,0,1,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,4,0,0,0,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,1,2,2,1,1,2,2,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,1,0,2,2,2, 2,1,0,0,0,0,1,0,0,0,2,2,2,2,0,4,0,0,1,0,0,1,0,0,0,0,0,0,2,2,2,0,0,0,0,0,1,0,0,2,2,2,0,0,1,1,0,0,0,0, 2,0,0,0,0,2,0,0,1,2,0,0,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2}
    };

    int map3[map_H][map_W] =
    {
        {0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    int map[map_H][map_W];


    if(CurrentLevel == 0)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map0[i][j];
            }
        }
    }
    if(CurrentLevel == 1)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map1[i][j];
            }
        }
    }
    if(CurrentLevel == 2)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map2[i][j];
            }
        }
    }
    if(CurrentLevel == 3)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map3[i][j];
            }
        }
    }



    float load = 0;

    for(short unsigned int i = 0; i < 10; i++)
    {
        for(short unsigned int j = 0; j < 150; j++)
        {
            glm::vec2 pos = {StandarSizeEntities * j + StandarSizeEntities * 5, StandarSizeEntities*i - map_H * StandarSizeEntities};
            #include <random>

            if(map[i][j] == -1)
            {
                Player* tmp = new Player("MAIN", PlayerMode::Normal, pos, glm::vec2(StandarSizeEntities), 0.0f, 0);
                OBJ_MN.Add(tmp);
                tmp->makeReference(&player);
                std::cout << "P CREATED" << std::endl; 
            }

            if(map[i][j] == 1)
            {
                std::random_device rd;

                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(0, 8);

                SolidBlock* tmp = new SolidBlock(dist(gen), pos, glm::vec2(StandarSizeEntities), -1);
                OBJ_MN.Add(tmp);
            }

            if(map[i][j] == 2)
            {
                std::random_device rd;

                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(0, 2);
                int ramdon = dist(gen);
                Spike* tmp = new Spike(static_cast<SpikeType>(ramdon), pos, glm::vec2(StandarSizeEntities), -1);
                OBJ_MN.Add(tmp);
            }

            if(map[i][j] == 3)
            {
                Orb* tmp = new Orb(OrbType::Green, pos, glm::vec2(StandarSizeEntities), -1);
                OBJ_MN.Add(tmp);
            }

            if(map[i][j] == 4)
            {
                Orb* tmp = new Orb(OrbType::Blue, pos, glm::vec2(StandarSizeEntities), -1);
                OBJ_MN.Add(tmp);
            }
        }
    }
}




