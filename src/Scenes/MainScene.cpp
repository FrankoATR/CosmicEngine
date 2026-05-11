#include "MainScene.hpp"

#include "../Entities/Player.hpp"
#include "../Entities/SolidBlock.hpp"
#include "../Entities/Spike.hpp"
#include "../Entities/Orb.hpp"


#include "../Utilities/Paths.hpp"

#include <WandEngine/Interfaces/Definitions.hpp>
#include <imgui/imgui.h>
#include <sstream>
#include <filesystem>

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



MainScene::MainScene() : GameScene("MainScene")
{
    currentMusic = "";
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
    EditorMode = false;
}


void MainScene::LoadResources()
{


}

void MainScene::Init()
{
    CAM_MN.SetCameraMode(CameraMode::CAMERA_2D);
    CAM_MN.SetFocusPosition(glm::vec2(-200.0f, -400.0f));

    //RS_MN.LoadShader("sprite", SHADER_SPRITE_VS, SHADER_SPRITE_FS);
    //RS_MN.LoadShader("sprite_sheet", SHADER_SPRITESHEET_VS, SHADER_SPRITESHEET_FS);

    RS_MN.LoadTexture("bg1", TEXTURE_GD_BG1, false);
    RS_MN.LoadTexture("g1", TEXTURE_GD_G1, true);
    RS_MN.LoadTexture("g2", TEXTURE_GD_G2, true);
    RS_MN.LoadTexture("line", TEXTURE_GD_LINE, true);
    RS_MN.LoadTextureSheet("gd", TEXTURESHEET_GD, true, 6, 6, 0);

    RS_MN.LoadTextFont("font1", FONT_THALEAHFAT, 30);

    //UI_MN.AddElement(new UIButton("BUTTON", nullptr, glm::vec2(1500.0f, 20.0f), glm::vec2(50.0f), true, nullptr));

    BOD_MN.SetNewGridArea(new GameGridCollisions(glm::vec2(-500.0f), 5, 5, 200));
    
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


    MSC_MN.Clear();

    if(fileName != "")
    {
        DB_MN.OpenDatabaseForLoading("levels/"+fileName);
    }
    else
    {
        DB_MN.OpenDatabaseForLoading("levels/lvl_mylevel");
    }

    std::string sql = "SELECT name FROM sqlite_master WHERE type='table';";


    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        std::string tableName = argv[0];

        if (tableName == "SolidBlock") {
            SolidBlock::LoadFrom();
        }
        else if (tableName == "Spike") {
            Spike::LoadFrom();
        }
        else if (tableName == "Orb") {
            Orb::LoadFrom();
        }
        else if (tableName == "Music") {
            std::string sql = "SELECT id, path FROM Music;";
    
            auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
                int id = std::stoi(argv[0]);

                MSC_MN.Clear();
                MSC_MN.Load("music", std::string(argv[1]));

                return 0;
            };
        
            DataBaseManager::GetInstance().ExecuteQuery(sql, callback, nullptr);
        }

        return 0;
    };

    DB_MN.ExecuteQuery(sql, callback, nullptr);

    DB_MN.CloseDatabase();


    
    player = new Player("MAIN", PlayerMode::Normal, glm::vec2(0.0f), glm::vec2(100.0f), 0.0f, 0);
    OBJ_MN.Add(player);
    player->makeReference(&player);

    MSC_MN.StopAll();
    MSC_MN.Play("music", ostVolume, false);

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

    if(player)
    {
        RS_MN.RenderText("Prueba De Texto", "font1", glm::vec3(player->GetPosition().x, player->GetPosition().y - 30, 0.0f), glm::vec3(2.0f));
    }

    if(EditorMode)
    {
        ResourceManager::GetInstance().RenderLine(
            glm::vec3(lineMusicPos, CAM_MN.GetPosition().y, 0.0f),
            glm::vec3(lineMusicPos, CAM_MN.GetPosition().y + CAM_MN.GetBaseWindowSize().y, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            1.0f,
            5.0f);
    }

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
            ImGui::Text("Select level:");
            std::string directoryPath = "levels";
            if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
                std::cerr << "Error: La ruta especificada no es un directorio válido." << std::endl;
                return;
            }
        
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (std::filesystem::is_regular_file(entry.path())) {
                    std::string name = entry.path().filename().string();

                    if (ImGui::Button(name.c_str())) {  
                        fileName = name;
                        Reset();
                    }
                }
            }

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

    MSC_MN.SetVolume("music", ostVolume);
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




    auto files = GM_MN.GetDroppedFiles();
    if(!files.empty())
    {
        std::cout << files.back() << std::endl;
        size_t dotPos = files.back().find_last_of(".");

        if (dotPos != std::string::npos) {
            std::string extension = files.back().substr(dotPos + 1);
            size_t lastSlash = files.back().find_last_of("/\\");
            currentMusic = (lastSlash == std::string::npos) ? files.back() : files.back().substr(lastSlash + 1);
            
            if (extension == "mp3")
            {
                droppedmusicfilepath = files.back();
                std::string destination = "assets/engine/audio/music/" + currentMusic;

                try {
                    std::filesystem::path srcPath = std::filesystem::absolute(droppedmusicfilepath);
                    std::filesystem::path destPath = std::filesystem::absolute(destination);
    
                    if (srcPath == destPath) {
                        std::cerr << "Error: El archivo de origen y destino son el mismo. No se copiará." << std::endl;
                    }
                    else
                    {
                        if (std::filesystem::exists(destination))
                        {
                            std::cout << "El archivo ya existe. Eliminando..." << std::endl;
                            std::filesystem::remove(destination);
                        }

                        std::filesystem::copy_file(droppedmusicfilepath, destination, std::filesystem::copy_options::overwrite_existing);
                    }
    

                    MSC_MN.Clear();
                    MSC_MN.StopAll();
                    MSC_MN.Load("music", destination);
                    MSC_MN.Play("music", ostVolume, false);

                } catch (const std::exception& e) {
                    std::cerr << "Error al copiar el archivo: " << e.what() << std::endl;
                }
            } 
        }
        else
        {
            droppedlevelfilepath = files.back();
            size_t lastSlash = droppedlevelfilepath.find_last_of("/\\");
            fileName = (lastSlash == std::string::npos) ? droppedlevelfilepath : droppedlevelfilepath.substr(lastSlash + 1);
            Reset();
        }

    }






    if(EditorMode)
    {
        if(player)
        {
            player->Destroy();
        }

        lineMusicPos += 1040.0f * deltaTime;

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
            if(fileName != "")
            {
                DB_MN.OpenDatabaseForSaving(fileName);
            }
            else
            {
                DB_MN.OpenDatabaseForSaving(("lvl_" + std::to_string(CurrentLevel)));
            }

            DataBaseManager::GetInstance().ExecuteSQL("BEGIN TRANSACTION;");

            SolidBlock::SaveToDB();
            Spike::SaveToDB();
            Orb::SaveToDB();

            DataBaseManager::GetInstance().CreateTable("Music", "id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT");
        
            std::ostringstream values;
            values << "'" << droppedmusicfilepath << "'";
            
            DataBaseManager::GetInstance().InsertData("Music", "path", values.str());
            
            DataBaseManager::GetInstance().ExecuteSQL("COMMIT;");
            DB_MN.CloseDatabase();

        }
        if(INP_MN.IsKeyPressed(GLFW_KEY_L, KeyDown))
        {
            Reset();

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

        for(int i = 0; i < 9; i++)
        {
            if(INP_MN.IsKeyPressed(GLFW_KEY_1+i, KeyDown))
            {
                if (selectedObject) selectedObject->Destroy();
                selectedObject = new SolidBlock(i, glm::vec2(objPos), glm::vec2(objSize), -1);
                selectedObject->makeReference(&selectedObject);
                OBJ_MN.Add(selectedObject);
            }
        }
        for(int i = 0; i < 3; i++)
        {
            if(INP_MN.IsKeyPressed(GLFW_KEY_RIGHT + i, KeyDown))
            {
                if (selectedObject) selectedObject->Destroy();
                selectedObject = new Spike(static_cast<SpikeType>(i), glm::vec2(objPos), glm::vec2(objSize), -1);
                selectedObject->makeReference(&selectedObject);
                OBJ_MN.Add(selectedObject);
            }
        }

        if(INP_MN.IsKeyPressed(GLFW_KEY_COMMA, KeyDown))
        {
            if (selectedObject) selectedObject->Destroy();
            selectedObject = new Orb(OrbType::Green, glm::vec2(objPos), glm::vec2(objSize), -1);
            selectedObject->makeReference(&selectedObject);
            OBJ_MN.Add(selectedObject);
        }

        if(INP_MN.IsKeyPressed(GLFW_KEY_PERIOD, KeyDown))
        {
            if (selectedObject) selectedObject->Destroy();
            selectedObject = new Orb(OrbType::Blue, glm::vec2(objPos), glm::vec2(objSize), -1);
            selectedObject->makeReference(&selectedObject);
            OBJ_MN.Add(selectedObject);
        }


        if(INP_MN.IsKeyPressed(GLFW_KEY_ENTER, KeyDown))
        {
            MSC_MN.SetPosition("music", CAM_MN.GetPosition().x * (1000.0f / 1040.0f));
            lineMusicPos =  CAM_MN.GetPosition().x;
        }

        if(selectedObject)
        {
            selectedObject->SetPosition(objPos);
            if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyDown))
            {
                auto objs = OBJ_MN.FindByMousePosition();
                if(objs.size() < 2)
                {
                    OBJ_MN.Add(selectedObject->Clone());
                }
            }
            if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2, KeyRelease))
            {
                auto objs = OBJ_MN.FindByMousePosition();
                if(objs.size() < 2)
                {
                    OBJ_MN.Add(selectedObject->Clone());
                }
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
            BOD_MN.SetGridPosition(glm::vec2(player->GetPosition() - glm::vec2(500.0f)));
        }
        else
        {
            BOD_MN.SetGridPosition(glm::vec2(-500.0f));
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
        if(EditorMode)
        {
            if(fileName != "")
            {
                DB_MN.OpenDatabaseForSaving("levels/"+fileName);
            }
            else
            {
                DB_MN.OpenDatabaseForSaving(("levels/lvl_mylevel"));
            }
            DataBaseManager::GetInstance().ExecuteSQL("BEGIN TRANSACTION;");

            SolidBlock::SaveToDB();
            Spike::SaveToDB();
            Orb::SaveToDB();

            DataBaseManager::GetInstance().CreateTable("Music", "id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT");
        
            std::ostringstream values;
            values << "'" << "assets/engine/audio/music/" + currentMusic << "'";
            
            DataBaseManager::GetInstance().InsertData("Music", "path", values.str());
            
            DataBaseManager::GetInstance().ExecuteSQL("COMMIT;");
            DB_MN.CloseDatabase();
        }
        
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




