#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace WandEngine
{

    enum class UIElementType
    {
        Button,
        Label,
        Image
    };

    class UIElement
    {
    protected:
        UIElementType ElementType;

        glm::vec2 Position;
        glm::vec2 Size;
        bool visible;

        UIElement *parent;
        std::vector<UIElement*> children;

    public:
        UIElement(glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent, UIElementType ElementType);
        virtual ~UIElement();

        virtual void Update(float deltaTime);
        virtual void Draw();

        void SetPosition(glm::vec2 NewPosition);
        void SetSize(glm::vec2 NewSize);
        void SetVisible(bool visible);
        bool IsVisible() const;

        void AddChild(UIElement* child);
        void RemoveChild(UIElement* child);
        void ClearChildren();

        void SetParent(UIElement *parent);
        UIElement *GetParent() const;

        bool MouseHover();
        
        glm::vec2 GetPosition() const;
        glm::vec2 GetSize() const;
        UIElementType GetType() const;
    };

}

#endif // UIELEMENT_HPP
