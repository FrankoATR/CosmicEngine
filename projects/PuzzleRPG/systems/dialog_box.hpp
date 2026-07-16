#ifndef PUZZLERPG_DIALOG_BOX_HPP
#define PUZZLERPG_DIALOG_BOX_HPP

#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>

namespace CosmicEngine { class UIImage; }

#include <cstdint>
#include <functional>
#include <string>

// Algunos headers de Windows (incluidos por enet/winsock2 -> windows.h)
// definen DialogBox como macro (DialogBoxA / DialogBoxW), lo que rompe la
// declaracion de nuestra clase. La deshacemos localmente.
#ifdef DialogBox
#undef DialogBox
#endif

namespace PuzzleRPG
{
    // Caja de dialogo. Aparece en la parte inferior de la pantalla y soporta
    // dos modos:
    //   - Informativo: muestra texto y se cierra al pulsar una tecla / clic.
    //   - Eleccion S/N: muestra dos botones "Si" / "No".
    //
    // Se construye una sola vez y se reutiliza durante la escena. Los eventos
    // se entregan mediante callbacks std::function configurables por el cliente.
    class DialogBox
    {
    public:
        using ChoiceCallback = std::function<void(bool yes)>;
        using DismissCallback = std::function<void()>;

        explicit DialogBox(const std::string &fontKey = "puzzle_font");
        ~DialogBox();

        // Configura las dimensiones / posicion en pixeles (coords UI).
        void Layout(glm::vec2 panelPosition, glm::vec2 panelSize);

        void ShowInfo(const std::string &text, DismissCallback onDismiss = nullptr);
        void ShowYesNo(const std::string &text, ChoiceCallback onChoice);
        void Hide();

        bool IsVisible() const { return visible; }
        std::uint32_t GetCurrentDialogId() const { return currentDialogId; }

        // Cierra el dialogo informativo si esta visible. Llamar desde la escena
        // cuando se detecte una tecla de "continuar".
        void DismissIfInfo();

    private:
        std::string fontKey;
        bool visible;
        bool isYesNo;
        std::uint32_t currentDialogId;

        CosmicEngine::UIImage *panelImage;
        CosmicEngine::UIText *textLabel;
        CosmicEngine::UIButton *yesButton;
        CosmicEngine::UIButton *noButton;

        ChoiceCallback  choiceCallback;
        DismissCallback dismissCallback;

        void SetButtonsVisible(bool value);
    };
}

#endif
