#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "../../Models/UI/UIElement.hpp"
#include "../../Models/UI/Derived/UIButton.hpp"
#include "../../Models/UI/Derived/UIText.hpp"
#include <vector>

namespace WandEngine
{

    class UIManager
    {
    private:
        UIManager();
        ~UIManager();
        UIManager(const UIManager &) = delete;
        UIManager &operator=(const UIManager &) = delete;

        std::vector<UIElement* > elements;
        bool MouseHoverAny;

    public:
        static UIManager &GetInstance();

        void Init();
        void Shutdown();
        void Update(float deltaTime);
        void Draw();

        bool IsMouseHoverAny();

        void AddElement(UIElement* element);
        void RemoveElement(UIElement* element);
        void Clear();

    };

}

#endif // UIMANAGER_HPP
