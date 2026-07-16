#pragma once

#include <glm/glm.hpp>
#include <random>

namespace PuzzleRPG
{
    class MenuBackground
    {
    public:
        static MenuBackground &Get();

        void LoadResources();
        void Init();
        void Update(double dt);
        void Draw();

    private:
        MenuBackground();
        ~MenuBackground();

        void ChooseNewDirection();

        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 velocity;
        double changeTimer;
        float speed;

        std::mt19937 rng;
        std::uniform_real_distribution<float> angleDist;

        const std::string textureKey = "puzzle_menu_bg";
    };
}
