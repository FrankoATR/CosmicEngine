#include "ui_manager.hpp"
#include <algorithm>
#include <iostream>

namespace CosmicEngine
{

    UIManager &UIManager::GetInstance()
    {
        static UIManager instance;
        return instance;
    }

    UIManager::UIManager()
    {
        std::cout << "UI manager created" << std::endl;
    }

    UIManager::~UIManager()
    {
        shutdown();
        std::cout << "UI manager destroyed" << std::endl;
    }


    void UIManager::init()
    {
        this->MouseHoverAny = false;
        std::cout << "UI manager initialized" << std::endl;
    }

    
    void UIManager::shutdown()
    {
        
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

        std::cout << "UI manager cleared" << std::endl;
        
    }


}
