#ifndef COSMIC_UIIMAGE_HPP
#define COSMIC_UIIMAGE_HPP

#include "../ui_element.hpp"
#include <string>
#include <glm/glm.hpp>

namespace CosmicEngine
{
    class UIImage : public UIElement
    {
    private:
        std::string textureKey;
        glm::vec3 tint;
        float alpha;

    public:
        UIImage(const std::string &textureKey, glm::vec2 Position, glm::vec2 Size, bool visible = true, UIElement* parent = nullptr);
        virtual ~UIImage();

        void update(float deltaTime) override;
        void draw() override;

        void SetTint(const glm::vec3 &t);
        void SetAlpha(float a);
    };

}

#endif // COSMIC_UIIMAGE_HPP
