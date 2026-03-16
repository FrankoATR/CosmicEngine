#include "main_scene.hpp"

#include "../entities/json_demo_object.hpp"
#include "../utilities/paths.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/storage/json/json_manager.hpp>
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
#include AUDIOMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER
#include SQLMANAGER_HEADER
#include JSONMANAGER_HEADER

#include <CosmicEngine/managers/resource/resource_manager.hpp>

MainScene::MainScene() : Scene("MainScene"), jsonSavePath("json_demo_save.json")
{
}

void MainScene::loadResources()
{
}

void MainScene::init()
{
    AUD_MN.Load("Music1", MUSIC_1_PATH, SoundType::Music);

    AUD_MN.SetPanRangeUnits(12.0f);

    AUD_MN.SetSfxVoicesPerSound(16);
    glm::vec3 listener = {0.0f, 0.0f, 0.0f};
    AUD_MN.SetListenerPosition(listener);
    AUD_MN.Play("Music1", 1.0f, false);

    //AUD_MN.PlayAt("test", glm::vec3(10.0f, 10.0f, 0.0f), SoundSpace::World2D, 1.0f);
    //AUD_MN.PlayAt("test", glm::vec3(10.0f, 10.0f, 10.0f), SoundSpace::World3D, 1.0f);

    //SCN_MN.SetBackgroundColor({0.0f, 0.0f, 0.0f});

    glm::vec2 pos(0.0f, 0.0f);
    CAM_MN.SetFocusPosition(pos);
    std::cout << CAM_MN.GetFocusPosition().x << ", " << CAM_MN.GetFocusPosition().y << std::endl;

    RS_MN.LoadFont("test_font", "assets/fonts/ThaleahFat.ttf", 32);
    RS_MN.LoadTexture("test_texture", "assets/textures/test.png");
    RS_MN.LoadTexture("test_texture2", "assets/textures/test.png");

    demoButton = new UIButton("Demo Button", "test_font", "test_texture2", {200.0f, 0.0f}, {150.0f, 50.0f}, false, true);

    UI_MN.AddElement(demoButton);


    JsonDemoObject::RegisterJsonSerialization();

    if (!JSON_MN.OpenFile(jsonSavePath))
    {
        std::cerr << "No se pudo abrir el archivo JSON: " << jsonSavePath << std::endl;
    }

    if (JSON_MN.HasClassSection(JsonDemoObject::StaticClassName()))
    {
        LoadJsonDemoObjects();
    }

    if (OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).empty())
    {
        CreateJsonDemoObject();
        SaveJsonDemoObjects();
    }

};

void MainScene::reset()
{
}

void MainScene::draw()
{
    //RS_MN.RenderRectangle({-40.0f, -100.0f}, {30.0f, 110.0f});
    //RS_MN.RenderText("Cosmic test", "test_font", {50.0f, 50.0f, 0.0f});
    //RS_MN.Render2DSprite("test_texture2", {0.0f, 0.0f}, {64.0f, 128.0f});
    RS_MN.RenderText("Mueve el objeto con flechas | G guarda | C carga", "test_font", {-360.0f, -180.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
    RS_MN.RenderText("Mueve la camara con WASD", "test_font", {-360.0f, -150.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
}

void MainScene::update(double deltaTime)
{
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE, KeyDown))
    {
        GameManager::GetInstance().endprogram();
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ENTER, KeyDown))
    {
        AUD_MN.SetPosition("test", 10000);
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_G, KeyDown))
    {
        SaveJsonDemoObjects();
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_C, KeyDown))
    {
        LoadJsonDemoObjects();
    }

    if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, KeyDown))
    {
        if(demoButton->MouseHover())
        {
            for (Object *object : OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()))
            {
                if (object)
                {
                        OBJ_MN.Remove(object->GetID());
                }
            }
        }
    }


    if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, KeyDown) && !UI_MN.IsMouseHoverAny())
    {
        glm::vec2 mousePos = INP_MN.GetMousePosition();

        for (Object *object : OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()))
        {
            if (object)
            {
                glm::vec2 objPos = object->GetPosition();
                glm::vec2 objSize = object->GetSize();

                if (mousePos.x >= objPos.x && mousePos.x <= objPos.x + objSize.x &&
                    mousePos.y >= objPos.y && mousePos.y <= objPos.y + objSize.y)
                {
                    std::cout << "Objeto " << object->GetID() << " clickeado!" << std::endl;
                    OBJ_MN.Remove(object->GetID());
                    break;
                }
            }
        }
    }

    if(INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT, KeyDown) && !UI_MN.IsMouseHoverAny())
    {
        glm::vec2 mousePos = INP_MN.GetMousePosition();

        JsonDemoObject *newObject = new JsonDemoObject(mousePos - glm::vec2(60.0f, 60.0f), {120.0f, 120.0f}, "Nuevo Objeto JSON", 100, 1);
        newObject->SetColor({0.9f, 0.3f, 0.5f});
        OBJ_MN.Add(newObject);
    }



    if (INP_MN.IsKeyPressed(GLFW_KEY_A, CosmicEngine::KeyRelease))
    {
        CAM_MN.SetFocusPosition(CAM_MN.GetFocusPosition() + glm::vec2(-10.0f, 0.0f));
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_D, CosmicEngine::KeyRelease))
    {
        CAM_MN.SetFocusPosition(CAM_MN.GetFocusPosition() + glm::vec2(10.0f, 0.0f));
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_W, CosmicEngine::KeyRelease))
    {
        CAM_MN.SetFocusPosition(CAM_MN.GetFocusPosition() + glm::vec2(0.0f, -10.0f));
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_S, CosmicEngine::KeyRelease))
    {
        CAM_MN.SetFocusPosition(CAM_MN.GetFocusPosition() + glm::vec2(0.0f, 10.0f));
    }

    if (INP_MN.IsKeyPressed(GLFW_KEY_F12, CosmicEngine::KeyDown))
    {
        GM_MN.enableVsync();
    }
    if (INP_MN.IsKeyPressed(GLFW_KEY_F11, CosmicEngine::KeyDown))
    {
        GM_MN.toggleFullscreen();
    }

}

void MainScene::CreateJsonDemoObject()
{
    JsonDemoObject *createdObject = new JsonDemoObject({-220.0f, -40.0f}, {120.0f, 120.0f}, "Objeto JSON", 100, 1);
    createdObject->SetColor({0.2f, 0.9f, 0.4f});
    OBJ_MN.Add(createdObject);
}

void MainScene::SaveJsonDemoObjects()
{

    JSON_MN.SaveObjectsData(JsonDemoObject::StaticClassName());

    if (JSON_MN.SaveFile(true))
    {
        std::cout << "Datos guardados en " << jsonSavePath << std::endl;
    }
    else
    {
        std::cerr << "No se pudieron guardar los datos JSON" << std::endl;
    }
}

void MainScene::LoadJsonDemoObjects()
{
    std::vector<Object *> existingObjects = OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName());
    for (Object *object : existingObjects)
    {
        if (object)
        {
            OBJ_MN.Remove(object->GetID());
        }
    }

    JSON_MN.LoadObjectsData(JsonDemoObject::StaticClassName());

    if (!OBJ_MN.FindByClassName(JsonDemoObject::StaticClassName()).empty())
    {
        std::cout << "Datos cargados desde " << jsonSavePath << std::endl;
    }
    else
    {
        std::cout << "No había objetos guardados para cargar" << std::endl;
    }
}
