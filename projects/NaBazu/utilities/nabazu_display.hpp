#ifndef NABAZU_DISPLAY_HPP
#define NABAZU_DISPLAY_HPP

#include "nabazu_input_actions.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER

#include <string>

namespace NaBazu
{
    // Shared F11 handler so fullscreen works from every scene (menu, gameplay and
    // game over) instead of only wherever it happened to be wired.
    //
    // VSync is deliberately re-applied right after the toggle: switching between
    // windowed and fullscreen recreates the swap chain, and the swap interval does
    // not reliably survive that on every driver -- without this, going fullscreen can
    // silently drop vsync and start tearing.
    inline void HandleDisplayToggles()
    {
        if (!CosmicEngine::InputManager::GetInstance().IsActionPressed(kActionToggleFullscreen, CosmicEngine::KeyDown))
        {
            return;
        }

        auto &game = CosmicEngine::GameManager::GetInstance();
        const bool vsyncWasEnabled = game.isVsyncEnabled();

        game.toggleFullscreen();

        if (vsyncWasEnabled)
        {
            game.enableVsync();
        }
        else
        {
            game.disableVsync();
        }
    }

    // Human-readable current display state, for menu/HUD readouts.
    inline std::string DescribeDisplayMode()
    {
        auto &game = CosmicEngine::GameManager::GetInstance();
        const std::string mode = game.isFullScreen() ? "Pantalla completa" : "Ventana";
        const std::string vsync = game.isVsyncEnabled() ? "VSync ON" : "VSync OFF";
        return mode + " | " + vsync;
    }
}

#endif
