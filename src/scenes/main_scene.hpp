#ifndef MAINSCENE_HPP
#define MAINSCENE_HPP

#include <CosmicEngine/Models/Scene/scene.hpp>
#include <CosmicEngine/Models/Object/object.hpp>
#include <CosmicEngine/Models/ui/ui_element.hpp>

#include <string>

using namespace CosmicEngine;

class Player;
class JsonDemoObject;

class MainScene : public Scene
{
private:
    std::string jsonSavePath;
    UIElement* demoButton;

    void CreateJsonDemoObject();
    void SaveJsonDemoObjects();
    void LoadJsonDemoObjects();


public:
    MainScene();
    
    void init() override;
    void reset() override;
    void draw() override;
    void update(double deltaTime) override;
    void loadResources() override;

};


#endif // MAINSCENE_HPP