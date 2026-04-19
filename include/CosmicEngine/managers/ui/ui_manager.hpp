#ifndef COSMIC_UIMANAGER_HPP
#define COSMIC_UIMANAGER_HPP

/**
 * @file ui_manager.hpp
 * @brief Declares the retained UI manager used by the engine.
 */

#include "../../models/ui/ui_element.hpp"
#include "../../models/ui/derived/ui_button.hpp"
#include "../../models/ui/derived/ui_text.hpp"
#include "../../models/ui/derived/ui_text_field.hpp"
#include <vector>

namespace CosmicEngine
{

    /**
     * @brief Manages retained UI elements, their updates, and their drawing order.
     *
     * The UIManager owns all active UI elements and processes their update/draw
     * calls each frame.  Use AddElement() to register new buttons, text labels,
     * or text fields, and RemoveElement() to unregister them.
     *
     * @par Example — registering UI elements
     * @code
     * auto* btn = new CosmicEngine::UIButton("Play", "font", "tex",
     *     {200, 0}, {150, 50}, false, true);
     * UI_MN.AddElement(btn);
     *
     * auto* label = new CosmicEngine::UIText("Title", "font",
     *     {0, 24}, {1920, 36}, true);
     * UI_MN.AddElement(label);
     *
     * // Check if mouse is over any interactive UI before spawning objects:
     * if (!UI_MN.IsMouseHoverAny())
     *     SpawnObjectAtMouse();
     * @endcode
     */
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
        /** @brief Returns the singleton instance of the UI manager. */
        static UIManager &GetInstance();

        /** @brief Initializes the UI manager state. */
        void init();
        /** @brief Shuts the UI manager down. */
        void shutdown();
        /**
         * @brief Updates all registered visible UI elements.
         * @param deltaTime Fixed update time step.
         */
        void update(float deltaTime);
        /** @brief Draws all registered UI elements. */
        void draw();

        /**
         * @brief Returns whether any non-label UI element is currently hovered.
         * @return True when at least one interactive UI element is hovered.
         */
        bool IsMouseHoverAny();

        /** @brief Registers a UI element with the manager. */
        void AddElement(UIElement* element);
        /** @brief Unregisters a UI element from the manager. */
        void RemoveElement(UIElement* element);
        /** @brief Deletes and removes every registered UI element. */
        void Clear();

    };

}

#endif // COSMIC_UIMANAGER_HPP
