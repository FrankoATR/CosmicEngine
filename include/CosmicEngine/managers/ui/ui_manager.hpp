#ifndef COSMIC_UIMANAGER_HPP
#define COSMIC_UIMANAGER_HPP

#include "../../models/ui/ui_element.hpp"
#include "../../models/ui/derived/ui_button.hpp"
#include "../../models/ui/derived/ui_text.hpp"
#include "../../models/ui/derived/ui_text_field.hpp"
#include <vector>

namespace CosmicEngine
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

        void init();
        void shutdown();
        void update(float deltaTime);
        void draw();

        bool IsMouseHoverAny();

        void AddElement(UIElement* element);
        void RemoveElement(UIElement* element);
        void Clear();

    };

}

#endif // COSMIC_UIMANAGER_HPP
