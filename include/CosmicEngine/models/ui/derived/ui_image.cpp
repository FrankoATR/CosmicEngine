#include "ui_image.hpp"
#include "../../../managers/resource/resource_manager.hpp"

namespace CosmicEngine
{
    UIImage::UIImage(const std::string &textureKey, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement *parent)
        : UIElement(Position, Size, visible, parent, UIElementType::Image), textureKey(textureKey), tint(glm::vec3(1.0f)), alpha(1.0f)
    {
    }

    UIImage::~UIImage()
    {
    }

    void UIImage::update(float deltaTime)
    {
        UIElement::update(deltaTime);
    }

    void UIImage::draw()
    {
        if (!visible) return;


        // Draw the texture stretched to exactly the element Size so callers
        // can reserve UI-space in pixels (e.g., 768x256) and have the image
        // cover that region entirely.
        glm::vec2 drawPos = Position;
        glm::vec2 drawSize = Size;

        ResourceManager::GetInstance().Render2DSpriteUnlit(textureKey, drawPos, drawSize, 0.0f, tint, alpha, ViewType::UI);

        UIElement::draw();
    }

    void UIImage::SetTint(const glm::vec3 &t)
    {
        tint = t;
    }

    void UIImage::SetAlpha(float a)
    {
        alpha = a;
    }

}
