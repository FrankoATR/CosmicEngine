/**
 * @file ui_manager.cpp
 * @brief Implements the retained UI manager used by the engine.
 */

#include "ui_manager.hpp"
#include "../input/input_manager.hpp"
#include "../../models/ui/derived/ui_text_field.hpp"
#include "../../utils/log.hpp"
#include <algorithm>

namespace CosmicEngine
{

    UIManager &UIManager::GetInstance()
    {
        static UIManager instance;
        return instance;
    }

    UIManager::UIManager()
    {
        RUNTIME_LIFECYCLE("UI manager", "created");
        usingMouseInput = true;
        lastMousePosUI = glm::vec2(0.0f);
    }

    UIManager::~UIManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("UI manager", "destroyed");
    }


    void UIManager::init()
    {
        this->MouseHoverAny = false;
        focusedElementIndex = -1;
        usingMouseInput = true;
        lastMousePosUI = InputManager::GetInstance().GetMousePosition_UI();
        RUNTIME_LIFECYCLE("UI manager", "initialized");
    }

    
    void UIManager::shutdown()
    {
        RUNTIME_LIFECYCLE("UI manager", "shutdown");
    }
    

    void UIManager::update(float deltaTime)
    {
        this->MouseHoverAny = false;

        const int focusedTextFieldIndex = FindFocusedTextFieldIndex();
        const bool textFieldCapturingInput = focusedTextFieldIndex != -1;
        if (textFieldCapturingInput && focusedElementIndex != focusedTextFieldIndex)
        {
            FocusElementAt(focusedTextFieldIndex);
        }

        // Determine input source: mouse movement/button vs keyboard/gamepad actions
        glm::vec2 currMouse = InputManager::GetInstance().GetMousePosition_UI();
        float dx = currMouse.x - lastMousePosUI.x;
        float dy = currMouse.y - lastMousePosUI.y;
        bool mouseMoved = (dx * dx + dy * dy) > 1.0f; // UI-space threshold (squared distance)
        bool mouseClicked = InputManager::GetInstance().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1, KeyEventType::KeyDown) ||
                            InputManager::GetInstance().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2, KeyEventType::KeyDown);
        bool keyboardNav = InputManager::GetInstance().IsActionPressed("ui_nav_next", KeyEventType::KeyDown) ||
                           InputManager::GetInstance().IsActionPressed("ui_nav_up", KeyEventType::KeyDown) ||
                           InputManager::GetInstance().IsActionPressed("ui_nav_down", KeyEventType::KeyDown) ||
                           InputManager::GetInstance().IsActionPressed("ui_nav_left", KeyEventType::KeyDown) ||
                           InputManager::GetInstance().IsActionPressed("ui_nav_right", KeyEventType::KeyDown) ||
                           InputManager::GetInstance().IsActionPressed("ui_submit", KeyEventType::KeyDown);

        bool newUsingMouse = usingMouseInput;
        if (textFieldCapturingInput)
        {
            newUsingMouse = true;
        }
        else if (mouseMoved || mouseClicked)
        {
            newUsingMouse = true;
        }
        else if (keyboardNav || InputManager::GetInstance().GetFirstKeyJustPressed() >= 0 || InputManager::GetInstance().GetFirstGamepadButtonJustPressed() >= 0)
        {
            newUsingMouse = false;
        }

        // React to input-mode change
        if (newUsingMouse != usingMouseInput)
        {
            usingMouseInput = newUsingMouse;
            if (usingMouseInput)
            {
                // switched to mouse: show cursor, clear persistent keyboard focus
                InputManager::GetInstance().SetDisableMouse(false);
                InputManager::GetInstance().SetActiveMouseInput();
                if (focusedElementIndex != -1)
                {
                    elements[focusedElementIndex]->SetFocused(false);
                    focusedElementIndex = -1;
                }
            }
            else
            {
                // switched to keyboard/gamepad: hide cursor and enable keyboard focus
                InputManager::GetInstance().SetDisableMouse(true);
                InputManager::GetInstance().SetInactiveMouseInput();
                if (focusedElementIndex == -1)
                {
                    FocusFirstInteractiveElement();
                }
            }
        }

        lastMousePosUI = currMouse;

        // If using mouse input, do not drive focus with mouse movement; rely on per-element hover.
        if (!usingMouseInput)
        {
            // keyboard/gamepad navigation drives focus
            UIElement *focusedElement = GetFocusedElement();
            const bool focusedTextField = dynamic_cast<UITextField *>(focusedElement) != nullptr;

            if (InputManager::GetInstance().IsActionPressed("ui_nav_next", KeyEventType::KeyDown))
            {
                FocusNextInteractiveElement(1);
            }

            if (!focusedTextField)
            {
                if (InputManager::GetInstance().IsActionPressed("ui_nav_up", KeyEventType::KeyDown) ||
                    InputManager::GetInstance().IsActionPressed("ui_nav_left", KeyEventType::KeyDown))
                {
                    FocusNextInteractiveElement(-1);
                }

                if (InputManager::GetInstance().IsActionPressed("ui_nav_down", KeyEventType::KeyDown) ||
                    InputManager::GetInstance().IsActionPressed("ui_nav_right", KeyEventType::KeyDown))
                {
                    FocusNextInteractiveElement(1);
                }
            }

            if (!focusedTextField && InputManager::GetInstance().IsActionPressed("ui_submit", KeyEventType::KeyDown))
            {
                focusedElement = GetFocusedElement();
                if (focusedElement)
                {
                    focusedElement->Activate();
                }
            }
        }

        // Always update elements (they will handle mouse clicks when appropriate)
        for (auto &element : elements)
        {
            if (!element || !element->IsVisible())
            {
                continue;
            }

            element->update(deltaTime);
            if( element->GetType() != UIElementType::Label && element->MouseHover())
            {
                this->MouseHoverAny = true;
            }
        }
    }

    int UIManager::FindFocusedTextFieldIndex() const
    {
        for (int index = 0; index < static_cast<int>(elements.size()); ++index)
        {
            UIElement *element = elements[index];
            auto *textField = dynamic_cast<UITextField *>(element);
            if (textField && textField->IsVisible() && textField->IsFocused())
            {
                return index;
            }
        }

        return -1;
    }

    void UIManager::draw()
    {
        for (auto &element : elements)
        {
            element->draw();
        }
    }

    bool UIManager::IsMouseHoverAny()
    {
        return this->MouseHoverAny;
    }


    void UIManager::AddElement(UIElement* element)
    {
        elements.push_back(element);
        if (focusedElementIndex == -1 && element && element->IsFocusable() && element->IsVisible() && !usingMouseInput)
        {
            FocusElementAt(static_cast<int>(elements.size()) - 1);
        }
    }

    void UIManager::RemoveElement(UIElement* element)
    {
        auto it = std::find(elements.begin(), elements.end(), element);
        if (it == elements.end())
        {
            return;
        }

        int removedIndex = static_cast<int>(std::distance(elements.begin(), it));
        if (removedIndex == focusedElementIndex)
        {
            element->SetFocused(false);
            focusedElementIndex = -1;
        }
        else if (removedIndex < focusedElementIndex)
        {
            --focusedElementIndex;
        }

        elements.erase(it);
    }

    void UIManager::Clear()
    {
        focusedElementIndex = -1;
        while(!elements.empty())
        {
            delete elements.back();
            elements.pop_back();
        }

        RUNTIME_LIFECYCLE("UI manager", "cleared");
        
    }

            UIElement *UIManager::GetFocusedElement() const
            {
                if (focusedElementIndex < 0 || focusedElementIndex >= static_cast<int>(elements.size()))
                {
                    return nullptr;
                }

                return elements[focusedElementIndex];
            }

            void UIManager::FocusElementAt(int index)
            {
                if (focusedElementIndex >= 0 && focusedElementIndex < static_cast<int>(elements.size()) && elements[focusedElementIndex])
                {
                    elements[focusedElementIndex]->SetFocused(false);
                }

                focusedElementIndex = -1;
                if (index < 0 || index >= static_cast<int>(elements.size()))
                {
                    return;
                }

                UIElement *element = elements[index];
                if (!element || !element->IsVisible() || !element->IsFocusable())
                {
                    return;
                }

                focusedElementIndex = index;
                element->SetFocused(true);
            }

            void UIManager::FocusFirstInteractiveElement()
            {
                for (int index = 0; index < static_cast<int>(elements.size()); ++index)
                {
                    UIElement *element = elements[index];
                    if (element && element->IsVisible() && element->IsFocusable())
                    {
                        FocusElementAt(index);
                        return;
                    }
                }
            }

            void UIManager::FocusNextInteractiveElement(int direction)
            {
                if (elements.empty())
                {
                    return;
                }

                if (focusedElementIndex == -1)
                {
                    FocusFirstInteractiveElement();
                    return;
                }

                const int elementCount = static_cast<int>(elements.size());
                for (int offset = 1; offset <= elementCount; ++offset)
                {
                    const int candidate = (focusedElementIndex + direction * offset + elementCount) % elementCount;
                    UIElement *element = elements[candidate];
                    if (element && element->IsVisible() && element->IsFocusable())
                    {
                        FocusElementAt(candidate);
                        return;
                    }
                }
            }

            void UIManager::SyncFocusFromMouse()
            {
                for (int index = 0; index < static_cast<int>(elements.size()); ++index)
                {
                    UIElement *element = elements[index];
                    if (element && element->IsVisible() && element->IsFocusable() && element->MouseHover())
                    {
                        if (focusedElementIndex != index)
                        {
                            FocusElementAt(index);
                        }
                        return;
                    }
                }
            }


}
