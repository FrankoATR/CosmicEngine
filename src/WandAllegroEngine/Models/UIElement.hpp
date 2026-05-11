#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <allegro5/allegro.h>
#include "../Interfaces/Definitions.hpp"

namespace WandEngine
{

    class UIElement
    {
    protected:
        UIElementType ElementType;

        WAND_VEC2 Position;
        WAND_VEC2 GlobalPosition;
        WAND_SIZE Size;
        bool visible;

        UIElement *parent;
        std::vector<UIElement*> children;

    public:
        UIElement(WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement* parent, UIElementType ElementType);
        virtual ~UIElement();

        virtual void Update(float deltaTime);
        virtual void Draw();

        void SetPosition(WAND_VEC2 NewPosition);
        void SetSize(WAND_SIZE NewSize);
        void SetVisible(bool visible);
        bool IsVisible() const;

        void AddChild(UIElement* child);
        void RemoveChild(UIElement* child);
        void ClearChildren();

        void SetParent(UIElement *parent);
        UIElement *GetParent() const;

        bool MouseHover();
        
        WAND_VEC2 GetPosition() const;
        WAND_SIZE GetSize() const;
        UIElementType GetType() const;
    };

}

#endif // UIELEMENT_HPP
