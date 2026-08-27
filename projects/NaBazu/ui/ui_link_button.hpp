#ifndef NABAZU_UI_LINK_BUTTON_HPP
#define NABAZU_UI_LINK_BUTTON_HPP

#include <CosmicEngine/models/ui/derived/ui_button.hpp>

#include <string>
#include <utility>

namespace NaBazu
{
    // A UIButton that is clickable with the mouse but NEVER takes keyboard focus.
    //
    // Why this exists: UIManager::AddElement auto-focuses the first focusable element
    // (when not in mouse mode) and its update() activates whatever is focused on the
    // "ui_submit" core action -- which is bound to ENTER. A plain UIButton on the
    // title screen therefore permanently swallowed ENTER, so the player could never
    // start the game. UIButton::HandleInput() resolves mouse clicks entirely on its
    // own (hover + mouse button, no focus involved), so opting out of focus costs
    // nothing and frees ENTER for the scene.
    class UILinkButton : public CosmicEngine::UIButton
    {
    public:
        UILinkButton(const std::string &text, const std::string &font, glm::vec2 position, glm::vec2 size)
            : CosmicEngine::UIButton(text, font, "", position, size)
        {
        }

        bool IsFocusable() const override { return false; }
    };
}

#endif
