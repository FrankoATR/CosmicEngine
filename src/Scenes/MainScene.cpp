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

MainScene::MainScene(int Level, int Attempts) : GameScene("MainScene")
{
    CurrentLevel = Level;
    currentMusic = "Electrodynamix";
    player = nullptr;
    ostVolume = 0.5f;
    fpsSliderValue = 60;
    ticksSliderValue = 20;
    vsynEnable = false;
    focusObjID = -1;
    mouseInitialPosForArea = glm::vec2(0.0f);
    mouseFinalPosForArea = glm::vec2(0.0f);
    mouseKeyDownForArea = false;
    cameraVelocity = glm::vec2(0.0f, 0.0f);
    showPanel = true;
}


void MainScene::LoadResources()
{


}

void MainScene::Init()
{
    CAM_MN.SetCameraMode(CameraMode::CAMERA_2D);
    CAM_MN.SetFocusPosition(glm::vec2(0.0f));

    //RS_MN.loadShader("sprite", SHADER_SPRITE_VS, SHADER_SPRITE_FS);
    //RS_MN.loadShader("sprite_sheet", SHADER_SPRITESHEET_VS, SHADER_SPRITESHEET_FS);

    //RS_MN.loadTexture("t1", TEXTURE_FIRSTBLOCK, true);
    //RS_MN.loadTexture("t2", TEXTURE_FACE, true);
    RS_MN.loadTextureSheet("gd", TEXTURESHEET_GD, true, 6, 6, 0);

    

    BOD_MN.SetNewGridArea(new GameGridCollisions(glm::vec2(-1000, -10 * 100), 25, 200, 100));
    
    LoadMap();


    MSC_MN.Load("Electrodynamix", MUSIC_GD_ELECTRODYNAMIX);
    MSC_MN.Load("Cycles", MUSIC_GD_CYCLES);
    MSC_MN.Load("Practice", MUSIC_GD_PRACTICE);

    SND_MN.Load("Dead", MUSIC_GD_DEAD);

    MSC_MN.Play(currentMusic, ostVolume, false);

    //SetProgressLoadingScene(1.0);
};

void MainScene::Draw()
{

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
            ImGui::Text("Press F3 to hidden panel");
            ImGui::Text("Press 1, 2 or 3 to select level");
            ImGui::Text("Press R to reset");
            ImGui::Text("Press H to show hitboxs");
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

        //CAM_MN.SetFocusObject(player); //agregarle offset
    }
    else
    {
        auto objs = OBJ_MN.GetAll();;
        for(auto &obj : objs)
        {
            obj->Destroy();
        }
        LoadMap();
        MSC_MN.StopAll();
        MSC_MN.Play(currentMusic, ostVolume, false);
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

    if(INP_MN.IsKeyPressed(GLFW_KEY_1, KeyDown))
    {
        CurrentLevel = 0;
        currentMusic = "Electrodynamix";
        player->Destroy();
    }
    if(INP_MN.IsKeyPressed(GLFW_KEY_2, KeyDown))
    {
        CurrentLevel = 1;
        currentMusic = "Cycles";
        player->Destroy();
    }
    if(INP_MN.IsKeyPressed(GLFW_KEY_3, KeyDown))
    {
        CurrentLevel = 2;
        currentMusic = "Practice";
        player->Destroy();
    }

    if(INP_MN.IsKeyPressed(GLFW_KEY_F11, KeyDown))
    {
        if(GM_MN.IsFullScreen())
            GM_MN.SetWindows_WindowsMode(960, 540);
        else
            GM_MN.SetWindows_FullScreenMode();
    }




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


    if(INP_MN.IsKeyPressed(GLFW_KEY_0, KeyDown))
    {
        //SCN_MN.ReplaceScene(new MainScene(0, 1));
    }


    if(INP_MN.IsKeyPressed(GLFW_KEY_R, KeyDown))
    {
        player->Destroy();
    }


/*
    if(InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT_CONTROL, KeyEventType::KeyDown))
        CameraManager::GetInstance().MovementSpeed = 5.0f;
    else
        CameraManager::GetInstance().MovementSpeed = 2.5f;
*/

}







void MainScene::LoadMap()
{

    srand(time(NULL));

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
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,2,2,2,1,2,2,1,0,0,0,0,0,2,2,0,0,0,0,0,0,0,1,2,2,2,2,2,0,0,0,0, 0,0,0,0,1,2,2,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,1,2,2,2,1,2,1,2,2,2,1,0,0,0,0,2, 2,2,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,1,0,0,0,0,0,0,0,1,2,1,1,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}
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
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,2,2,2,2,1,1,0,1,1,0,0,0,0,0,1,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,0,0,0,1,2,1,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,2,1,0,0,0, 0,0,0,0,2,2,2,1,2,0,0,0,0,0,1,2,2,2,1,2,1,0,0,0,0,0,1,2,2,2,1,1,1,1,2,1,1,1,0,1,0,0,0,0,0,0,1,2,2,2}
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
        {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,1,2,2,1,1,2,2,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,1,0,2,2,2, 2,1,0,0,0,0,1,0,0,0,2,2,2,2,0,4,0,0,1,0,0,1,0,0,0,0,0,0,2,2,2,0,0,0,0,0,1,0,0,2,2,2,0,0,1,1,0,0,0,0, 2,0,0,0,0,2,0,0,1,2,0,0,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2}
    };

    int map[map_H][map_W];


    if(CurrentLevel == 0)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map0[i][j];
            }
        }

        currentMusic = "Electrodynamix";
    }
    if(CurrentLevel == 1)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map1[i][j];
            }
        }

        currentMusic = "Cycles";
    }
    if(CurrentLevel == 2)
    {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 150; ++j) {
                map[i][j] = map2[i][j];
            }
        }

        currentMusic = "Practice";
    }



    float load = 0;

    for(short unsigned int i = 0; i < 10; i++)
    {
        for(short unsigned int j = 0; j < 150; j++)
        {
            glm::vec2 pos = {StandarSizeEntities * j + StandarSizeEntities * 5, StandarSizeEntities*i - map_H * StandarSizeEntities};

            if(map[i][j] == -1)
            {
                Player* tmp = new Player("MAIN", PlayerMode::Normal, pos, glm::vec2(StandarSizeEntities), 0.0f, 0);
                OBJ_MN.Add(tmp);
                tmp->makeReference(&player);
            }

            if(map[i][j] == 1)
            {
                SolidBlock* tmp = new SolidBlock(pos, glm::vec2(StandarSizeEntities), -1);
                OBJ_MN.Add(tmp);
            }

            if(map[i][j] == 2)
            {
                Spike* tmp = new Spike(pos, glm::vec2(StandarSizeEntities), -1);
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
