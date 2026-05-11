#include "SecondScene3D.hpp"

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

#include <WandEngine/Managers/Resource/ResourceManager.hpp>

SecondScene3D::SecondScene3D() : Scene("SecondScene3D")
{

}

void SecondScene3D::LoadResources()
{
}

void SecondScene3D::Init()
{

    SCN_MN.SetBackgroundColor(glm::vec3(0.0f));
    
};

void SecondScene3D::Reset()
{


}

void SecondScene3D::Draw()
{
    glm::vec3 position(0.0f);
    glm::vec3 size(5.0f);

    ResourceManager::GetInstance().RenderRectangle(
        glm::vec3(position), 
        glm::vec3(position + size), 
        glm::vec3(position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2), 
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        1.0f, 
        5.0f);

}

void SecondScene3D::Update(double deltaTime)
{
    std::cout << CAM_MN.front.x << ", " << CAM_MN.front.y << ", " << CAM_MN.front.z << std::endl;

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
}
