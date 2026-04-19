/**
 * @file ui_manager.cpp
 * @brief Implements the retained UI manager used by the engine.
 */

#include "ui_manager.hpp"
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
    }

    UIManager::~UIManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("UI manager", "destroyed");
    }


    void UIManager::init()
    {
        this->MouseHoverAny = false;
        RUNTIME_LIFECYCLE("UI manager", "initialized");
    }

    
    void UIManager::shutdown()
    {
        RUNTIME_LIFECYCLE("UI manager", "shutdown");
    }
    

    void UIManager::update(float deltaTime)
    {
        this->MouseHoverAny = false;
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
    }

    void UIManager::RemoveElement(UIElement* element)
    {
        elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
    }

    void UIManager::Clear()
    {
        while(!elements.empty())
        {
            delete elements.back();
            elements.pop_back();
        }

        RUNTIME_LIFECYCLE("UI manager", "cleared");
        
    }


}
