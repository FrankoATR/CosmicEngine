#ifndef SECONDSCENE3D_HPP
#define SECONDSCENE3D_HPP

#include <WandEngine/Models/Scene/Scene.hpp>
#include <WandEngine/Models/Object/Object.hpp>

using namespace WandEngine;

class Player;

class SecondScene3D : public Scene
{
private:


public:
    SecondScene3D();
    
    void Init() override;
    void Reset() override;
    void Draw() override;
    void Update(double deltaTime) override;
    void LoadResources() override;

};


#endif // SECONDSCENE3D_HPP