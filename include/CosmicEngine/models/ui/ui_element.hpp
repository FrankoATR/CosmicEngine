#ifndef COSMIC_UIELEMENT_HPP
#define COSMIC_UIELEMENT_HPP

/**
 * @file ui_element.hpp
 * @brief Declares the base type for retained UI elements.
 */

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace CosmicEngine
{

    /**
     * @brief Identifies the concrete type of a UI element.
     */
    enum class UIElementType
    {
        Button,
        Label,
        Image,
        TextField
    };

    /**
     * @brief Base class for all retained UI elements managed by the engine.
     *
     * UIElement provides common position, size, visibility, parent-child hierarchy,
     * and mouse-hover detection.  Concrete elements (UIButton, UIText, UITextField)
     * derive from this class and are registered with the UIManager.
     *
     * @par Example — checking mouse hover on any element
     * @code
     * if (myElement->IsVisible() && myElement->MouseHover())
     *     std::cout << "Mouse is over the element" << std::endl;
     * @endcode
     */
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
        /**
         * @brief Creates a UI element.
         * @param Position Element position in UI space.
         * @param Size Element size.
         * @param visible Initial visibility state.
         * @param parent Optional parent element.
         * @param ElementType Concrete UI element type.
         */
        UIElement(glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent, UIElementType ElementType);
        /** @brief Releases the UI element and its owned relationships. */
        virtual ~UIElement();

        /**
         * @brief Updates the UI element state.
         * @param deltaTime Fixed update time step.
         */
        virtual void update(float deltaTime);
        /** @brief Draws the UI element. */
        virtual void draw();

        /** @brief Sets the element position in UI space. */
        void SetPosition(glm::vec2 NewPosition);
        /** @brief Sets the element size. */
        void SetSize(glm::vec2 NewSize);
        /** @brief Sets the visibility state of the element. */
        void SetVisible(bool visible);
        /** @brief Returns whether the element is currently visible. */
        bool IsVisible() const;

        /** @brief Adds a child to the current UI element. */
        void AddChild(UIElement* child);
        /** @brief Removes a child from the current UI element. */
        void RemoveChild(UIElement* child);
        /** @brief Removes all child elements from the current UI element. */
        void ClearChildren();

        /** @brief Sets the parent element. */
        void SetParent(UIElement *parent);
        /** @brief Returns the parent element. */
        UIElement *GetParent() const;

        /**
         * @brief Returns whether the mouse cursor is currently hovering the element.
         * @return True when the cursor lies inside the element bounds.
         */
        bool MouseHover();
        
        /** @brief Returns the element position in UI space. */
        glm::vec2 GetPosition() const;
        /** @brief Returns the element size. */
        glm::vec2 GetSize() const;
        /** @brief Returns the concrete UI element type. */
        UIElementType GetType() const;
    };

}

#endif // COSMIC_UIELEMENT_HPP
