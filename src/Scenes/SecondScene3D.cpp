#include "SecondScene3D.hpp"

#include "../Entities/Cube.hpp"

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

#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Light/LightManager.hpp>

SecondScene3D::SecondScene3D() : Scene("SecondScene3D")
{

}

void SecondScene3D::LoadResources()
{
}

void SecondScene3D::Init()
{
    GM_MN.SetTargetFPS(144);
    GM_MN.EnableVsync();
    
    GM_MN.SetMousePosition_Callback([](double x, double y){
        CAM_MN.Classic3DProcessMouseMovement(x, y);
    });

    GM_MN.SetMouseScroll_Callback([](double x, double y){
        CAM_MN.Classic3DProcessMouseScroll(x, y);
    });

    SCN_MN.SetBackgroundColor(glm::vec3(0.0f, 0.66f, 0.89f));
    
    RS_MN.GetInstance().Load3DModel("mona", MODEL_MONA);
    RS_MN.LoadTexture("cobblestone", TEXTURE_COBBLESTONE_MINECRAFT, true);
    
    LightManager::GetInstance().Add(new Light(glm::vec3(0.0f, 10.0f, 0.0f)));
    LightManager::GetInstance().Add(new Light(glm::vec3(5.0f, 5.0f, 0.0f)));
    LightManager::GetInstance().Add(new Light(glm::vec3(0.0f, 5.0f, -5.0f)));
    LightManager::GetInstance().Add(new Light(glm::vec3(4.0f, 10.0f, 3.0f)));

    MSC_MN.GetInstance().Load("mt moon", MUSIC_MT_MOON);
    //MSC_MN.GetInstance().Play("mt moon", 1.0f, false);

    glm::vec3 position3d(0.0f);

    for(int i = 0; i < 1; i++)
    {
        for(int j = 0; j < 1; j++)
        {
            position3d.x = i;
            position3d.z = j;
            OBJ_MN.Add( new Cube(position3d, glm::vec3(1.0f)) );
        }
    }


    RS_MNX()->LoadTexture("btn1", TEXTURE_TETRIS_BTN1, true);
    RS_MN.LoadTextFont("font1", FONT_THALEAHFAT, 25);

    UIButton *Btn_1 = new UIButton(":D", "font2", "btn1", glm::vec2(1200.0f, 20.0f), glm::vec2(300.0f, 150.0f));
    Btn_1->SetOnClick([Btn_1]()
                      {
        std::cout << "CLICK" << std::endl;    
        Btn_1->SetText(std::to_string(GameManager::GetInstance().GetCurrentFPS())); });
    UI_MN.AddElement(Btn_1);

};

void SecondScene3D::Reset()
{


}

void SecondScene3D::Draw()
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

        ImGui::Text("Current position: x: %.2f, y: %.2f, z: %.2f",
            CAM_MN.GetPosition().x,
            CAM_MN.GetPosition().y,
            CAM_MN.GetPosition().z);

        ImGui::Text("Current view: x: %.2f, y: %.2f, z: %.2f",
            CAM_MN.GetViewDirection().x,
            CAM_MN.GetViewDirection().y,
            CAM_MN.GetViewDirection().z);

        ImGui::PopStyleColor();
    }
    ImGui::End();

/*
    glm::vec3 position(0.0f);
    glm::vec3 color(0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 size(1.0f);


    for(int i = 1; i < 30; i++)
    {
        color.x += cos(glfwGetTime()) + 0.5;
        color.y += sin(glfwGetTime()) + 0.5;
        color.z += cos(glfwGetTime()) + 0.5;
        rotation.y += sin(glfwGetTime());
        rotation.x += cos(glfwGetTime());
        rotation.z += sin(glfwGetTime());

        ResourceManager::GetInstance().RenderRectangle(
            glm::vec3(position), 
            glm::vec3(position + size), 
            glm::vec3(position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2), 
            glm::vec3(rotation), 
            glm::vec3(color), 
            1.0f, 
            5.0f);

    }



    glm::vec3 position3d(0.0f);
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            for(int p = 0; p < 5; p++)
            {
                position3d.x = i;
                position3d.y = j;
                position3d.z = p;
                RS_MN.Render3DModel("mona", position3d, glm::vec3(0.5f));
            }
        }
    }

*/
    RS_MN.RenderParallelepipedLines(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.5f, 1.0f, 2.0f), glm::vec3(0.0f, 2.0f, 0.0f), rotation, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 5.0f);


}

void SecondScene3D::Update(double deltaTime)
{

    rotation.x += deltaTime * 100.0f;
    rotation.y += deltaTime * 50.0f;
    rotation.z -= deltaTime * 75.0f;

    if(INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, KeyDown))
    {
        GM_MN.EndProgram();
    }

    if(INP_MN.IsKeyPressed(GLFW_KEY_W, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(FORWARD, deltaTime);
    }
    else if(INP_MN.IsKeyPressed(GLFW_KEY_S, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(BACKWARD, deltaTime);
    }
    if(INP_MN.IsKeyPressed(GLFW_KEY_A, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(LEFT, deltaTime);
    }
    else if(INP_MN.IsKeyPressed(GLFW_KEY_D, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(RIGHT, deltaTime);
    }
    if(INP_MN.IsKeyPressed(GLFW_KEY_SPACE, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(UP, deltaTime);
    }
    else if(INP_MN.IsKeyPressed(GLFW_KEY_LEFT_SHIFT, KeyRelease))
    {
        CAM_MN.Classic3DProcessKeyboard(DOWN, deltaTime);
    }

    if(INP_MN.IsKeyPressed(GLFW_KEY_LEFT_CONTROL, KeyRelease))
    {
        CAM_MN.SetMovementSpeed(5.0f);
    }
    else
    {
        CAM_MN.SetMovementSpeed(2.5f);
    }


    if (INP_MN.IsKeyPressed(GLFW_KEY_F11, KeyDown))
    {
        if (GM_MN.IsFullScreen())
            GM_MN.SetWindows_WindowsMode(960, 540);
        else
            GM_MN.SetWindows_FullScreenMode();
    }
}
