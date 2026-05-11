#include "UIElement.hpp"
#include "../Managers/GameManager.hpp"
#include "../Managers/Camera/CameraManager.hpp"
#include "../Managers/Input/InputManager.hpp"

#include <algorithm>

namespace WandEngine
{

    UIElement::UIElement(glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent, UIElementType ElementType) : Position(Position), 
        GlobalPosition(Position), Size(Size), visible(visible), parent(parent), ElementType(ElementType)
    {
        
    }

    UIElement::~UIElement()
    {
        ClearChildren();
    }

    void UIElement::Update(float deltaTime)
    {
        GlobalPosition.x = Position.x + CameraManager::GetInstance().GetFocusPosition().x - GameManager::GetInstance().GetWindowsSize().x/2;
        GlobalPosition.y = Position.y + CameraManager::GetInstance().GetFocusPosition().y + GameManager::GetInstance().GetWindowsSize().y/2;
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
        glm::vec2 CameraFocusPosition = CameraManager::GetInstance().GetFocusPosition();
        glm::vec2 ScreenSize = GameManager::GetInstance().GetWindowsSize();
        glm::vec2 CameraPosition = glm::vec2(CameraFocusPosition.x - ScreenSize.x/2, CameraFocusPosition.y + ScreenSize.y/2);
        glm::vec2 MousePosition = InputManager::GetInstance().GetMousePosition();
        return MousePosition.x >= Position.x + CameraPosition.x && MousePosition.x <= Position.x + CameraPosition.x + Size.x && MousePosition.y >= Position.y + CameraPosition.y && MousePosition.y <= Position.y + CameraPosition.y + Size.y;
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
