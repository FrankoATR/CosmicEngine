#include "UIManager.hpp"

namespace WandEngine
{

    UIManager::UIManager()
    {
        this->MouseHoverAny = false;
    }

    UIManager::~UIManager()
    {
        Clear();
        std::cout << "UI manager destroyed" << std::endl;
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
