/**
 * @file ui_element.cpp
 * @brief Implements the base retained UI element used by the engine.
 */

#include "ui_element.hpp"
#include "../../managers/game_manager.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"
#include "../../utils/log.hpp"

#include <algorithm>

namespace CosmicEngine
{
    std::string_view UIElement::GetTypeName(UIElementType type)
    {
        switch (type)
        {
        case UIElementType::Button:
            return "UIButton";
        case UIElementType::Label:
            return "UILabel";
        case UIElementType::Image:
            return "UIImage";
        case UIElementType::TextField:
            return "UITextField";
        default:
            return "UIElement";
        }
    }

    UIElement::UIElement(glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent, UIElementType ElementType) : Position(Position), 
        Size(Size), visible(visible), parent(parent), ElementType(ElementType)
    {
        RUNTIME_LIFECYCLE(GetTypeName(this->ElementType), "created");
    }

    UIElement::~UIElement()
    {
        ClearChildren();
		RUNTIME_LIFECYCLE(GetTypeName(this->ElementType), "destroyed");
    }

    void UIElement::update(float deltaTime)
    {
        for (auto &child : children)
        {
            child->update(deltaTime);
        }
    }

    void UIElement::draw()
    {
        if (!visible)
        {
            return;
        }

        for (auto &child : children)
        {
            child->draw();
        }
    }

    void UIElement::SetPosition(glm::vec2 NewPosition)
    {
        this->Position = NewPosition;
    }

    void UIElement::SetSize(glm::vec2 NewSize)
    {
        this->Size = NewSize;
    }

    void UIElement::SetVisible(bool visible)
    {
        this->visible = visible;
    }

    bool UIElement::IsVisible() const
    {
        return visible;
    }

    void UIElement::AddChild(UIElement *child)
    {
        child->SetParent(this);
        children.push_back(child);
    }

    void UIElement::RemoveChild(UIElement *child)
    {
        child->SetParent(nullptr);
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void UIElement::ClearChildren()
    {
        for (auto &child : children)
        {
            child->SetParent(nullptr);
        }
        children.clear();
    }

    void UIElement::SetParent(UIElement *parent)
    {
        this->parent = parent;
    }

    UIElement *UIElement::GetParent() const
    {
        return parent;
    }

    bool UIElement::MouseHover()
    {
        if (!visible)
        {
            return false;
        }

        glm::vec2 MousePosition = InputManager::GetInstance().GetMousePosition_UI();
        return MousePosition.x >= Position.x && MousePosition.x <= Position.x + Size.x && MousePosition.y >= Position.y && MousePosition.y <= Position.y + Size.y;
    }

    glm::vec2 UIElement::GetPosition() const
    {
        return this->Position;
    }

    glm::vec2 UIElement::GetSize() const
    {
        return this->Size;
    }

    UIElementType UIElement::GetType() const
    {
        return this->ElementType;
    }

}
