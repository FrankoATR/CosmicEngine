#ifndef SECONDSCENE3D_HPP
#define SECONDSCENE3D_HPP

#include <WandEngine/Models/Scene/Scene.hpp>
#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Model/Model.hpp>
#include <WandEngine/Models/Light/Light.hpp>

using namespace WandEngine;

class Player;

class SecondScene3D : public Scene
{
private:
    Model* tmp;
    Light* light;
    glm::vec3 rotation = glm::vec3(0.0f);

public:
    SecondScene3D();
    
    void Init() override;
    void Reset() override;
    void Draw() override;
    void Update(double deltaTime) override;
    void LoadResources() override;

};


#endif // SECONDSCENE3D_HPP