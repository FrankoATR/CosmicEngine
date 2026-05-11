#include "UIManager.hpp"
#include <algorithm>
#include <iostream>

namespace WandEngine
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
        Shutdown();
        std::cout << "UI manager destroyed" << std::endl;
    }


    void UIManager::Init()
    {
        this->MouseHoverAny = false;
        std::cout << "UI manager initialized" << std::endl;
    }

    
    void UIManager::Shutdown()
    {
        
    }
    

    void UIManager::Update(float deltaTime)
    {
        this->MouseHoverAny = false;
        for (auto &element : elements)
        {
            element->Update(deltaTime);
            if( element->GetType() != UIElementType::Label && element->MouseHover())
            {
                this->MouseHoverAny = true;
            }
        }
    }

    void UIManager::Draw()
    {
        for (auto &element : elements)
        {
            element->Draw();
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
