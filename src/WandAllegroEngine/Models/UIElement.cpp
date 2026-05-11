#include "UIElement.hpp"
#include "../Managers/CameraManager.hpp"
#include "../Managers/InputManager.hpp"

namespace WandEngine
{

    UIElement::UIElement(WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement* parent, UIElementType ElementType) : Position(Position), 
        GlobalPosition(Position), Size(Size), visible(visible), parent(parent), ElementType(ElementType)
    {
        
    }

    UIElement::~UIElement()
    {
        ClearChildren();
    }

    void UIElement::Update(float deltaTime)
    {
        GlobalPosition.x = Position.x + CameraManager::GetInstance().GetPosition().x;
        GlobalPosition.y = Position.y + CameraManager::GetInstance().GetPosition().y ;
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

    void UIElement::SetPosition(WAND_VEC2 NewPosition)
    {
        this->Position = NewPosition;
    }

    void UIElement::SetSize(WAND_SIZE NewSize)
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
        WAND_VEC2 CameraPosition = CameraManager::GetInstance().GetPosition();
        WAND_VEC2 MousePosition = InputManager::GetInstance().GetMousePosition();
        return MousePosition.x >= Position.x + CameraPosition.x && MousePosition.x <= Position.x + CameraPosition.x + Size.width && MousePosition.y >= Position.y + CameraPosition.y && MousePosition.y <= Position.y + CameraPosition.y + Size.height;
    }

    WAND_VEC2 UIElement::GetPosition() const
    {
        return this->Position;
    }

    WAND_SIZE UIElement::GetSize() const
    {
        return this->Size;
    }

    UIElementType UIElement::GetType() const
    {
        return this->ElementType;
    }

}
