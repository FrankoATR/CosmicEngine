#include "UIElement.hpp"
#include "../../Managers/GameManager.hpp"
#include "../../Managers/Camera/CameraManager.hpp"
#include "../../Managers/Input/InputManager.hpp"

#include <algorithm>
#include <iostream>

namespace WandEngine
{

    UIElement::UIElement(glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent, UIElementType ElementType) : Position(Position), 
        Size(Size), visible(visible), parent(parent), ElementType(ElementType)
    {
        
    }

    UIElement::~UIElement()
    {
        ClearChildren();
    }

    void UIElement::Update(float deltaTime)
    {
        for (auto &child : children)
        {
            child->Update(deltaTime);
        }
    }

    void UIElement::Draw()
    {
        if (!visible)
        {
            return;
        }

        for (auto &child : children)
        {
            child->Draw();
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
