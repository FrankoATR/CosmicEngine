#include "dialog_box.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include UIMANAGER_HEADER
#include <CosmicEngine/models/ui/derived/ui_image.hpp>

namespace PuzzleRPG
{
    DialogBox::DialogBox(const std::string &fontKey)
        : fontKey(fontKey),
          visible(false),
          isYesNo(false),
          currentDialogId(0),
          panelImage(nullptr),
          textLabel(nullptr),
          yesButton(nullptr),
          noButton(nullptr)
    {
        // Layout por defecto (1920x1080 base de render).
        // Use the original dialog texture pixel size (768x256) and center
        // it horizontally, slightly above the bottom of a 1920x1080 base.
        Layout(glm::vec2((1920.0f - 768.0f) * 0.5f, 1080.0f - 256.0f - 24.0f), glm::vec2(768.0f, 256.0f));
    }

    DialogBox::~DialogBox()
    {
        // UIManager es duenyo de los elementos: cuando la escena se libera
        // llama a UIManager::Clear() que ya hace delete sobre cada elemento.
        // Liberar aqui causaria double-free y cerraria la ventana al volver
        // al menu principal. Solo desreferenciamos.
        panelImage = nullptr;
        textLabel  = nullptr;
        yesButton  = nullptr;
        noButton   = nullptr;
    }

    void DialogBox::Layout(glm::vec2 panelPosition, glm::vec2 panelSize)
    {
        if (!panelImage)
        {
            panelImage = new CosmicEngine::UIImage("ui/message", panelPosition, panelSize, false);
            UI_MN.AddElement(panelImage);
        }
        if (!textLabel)
        {
            // Multiline left-aligned text, increased padding and max lines.
            // Move start further right/down to avoid clipping with the panel
            // border, and reduce available area accordingly so text never
            // overflows. Allow up to 7 visible lines.
            textLabel = new CosmicEngine::UIText("", fontKey,
                                                 panelPosition + glm::vec2(56.0f, 44.0f),
                                                 panelSize - glm::vec2(112.0f, 88.0f),
                                                 false, nullptr, true, 7, true);
            textLabel->SetTextColor(glm::vec3(0.0f));
            UI_MN.AddElement(textLabel);
        }
        if (!yesButton)
        {
            {
                std::string label = "Si";
                auto choose_idle = [](const std::string &s) {
                    size_t l = s.size();
                    if (l <= 4) return std::string("ui/button-idle");
                    if (l <= 16) return std::string("ui/button-idle-m");
                    return std::string("ui/button-idle-l");
                };
                std::string idleKey = choose_idle(label);
                std::string hoverKey = idleKey; auto p = hoverKey.find("idle"); if (p != std::string::npos) hoverKey.replace(p, 4, "hover");
                yesButton = new CosmicEngine::UIButton(label, fontKey, idleKey,
                                                       panelPosition + glm::vec2(panelSize.x - 380.0f, panelSize.y - 80.0f),
                                                       glm::vec2(170.0f, 60.0f), true, false);
                yesButton->SetHoverTexture(hoverKey);
            }
            yesButton->SetTextureScale(1.06f);
            yesButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
            UI_MN.AddElement(yesButton);
                yesButton->SetOnClick([this]()
            {
                visible = false;
                isYesNo = false;
                SetButtonsVisible(false);
                if (panelImage) panelImage->SetVisible(false);
                if (textLabel) textLabel->SetVisible(false);
                if (choiceCallback) { auto cb = choiceCallback; choiceCallback = nullptr; cb(true); }
            });
        }
        if (!noButton)
        {
            {
                std::string label = "No";
                auto choose_idle = [](const std::string &s) {
                    size_t l = s.size();
                    if (l <= 4) return std::string("ui/button-idle");
                    if (l <= 16) return std::string("ui/button-idle-m");
                    return std::string("ui/button-idle-l");
                };
                std::string idleKey = choose_idle(label);
                std::string hoverKey = idleKey; auto p = hoverKey.find("idle"); if (p != std::string::npos) hoverKey.replace(p, 4, "hover");
                noButton = new CosmicEngine::UIButton(label, fontKey, idleKey,
                                                      panelPosition + glm::vec2(panelSize.x - 200.0f, panelSize.y - 80.0f),
                                                      glm::vec2(170.0f, 60.0f), true, false);
                noButton->SetHoverTexture(hoverKey);
            }
            noButton->SetTextureScale(1.06f);
            noButton->SetTextOffset(glm::vec2(0.0f, -4.0f));
            UI_MN.AddElement(noButton);
            noButton->SetOnClick([this]()
            {
                visible = false;
                isYesNo = false;
                SetButtonsVisible(false);
                if (panelImage) panelImage->SetVisible(false);
                if (textLabel) textLabel->SetVisible(false);
                if (choiceCallback) { auto cb = choiceCallback; choiceCallback = nullptr; cb(false); }
            });
        }
    }

    void DialogBox::SetButtonsVisible(bool value)
    {
        if (yesButton) yesButton->SetVisible(value);
        if (noButton)  noButton->SetVisible(value);
    }

    void DialogBox::ShowInfo(const std::string &text, DismissCallback onDismiss)
    {
        ++currentDialogId;
        isYesNo = false;
        visible = true;
        dismissCallback = std::move(onDismiss);
        choiceCallback = nullptr;
        if (panelImage) panelImage->SetVisible(true);
        if (textLabel) { textLabel->SetText(text); textLabel->SetVisible(true); }
        SetButtonsVisible(false);
    }

    void DialogBox::ShowYesNo(const std::string &text, ChoiceCallback onChoice)
    {
        ++currentDialogId;
        isYesNo = true;
        visible = true;
        choiceCallback = std::move(onChoice);
        dismissCallback = nullptr;
        if (panelImage) panelImage->SetVisible(true);
        if (textLabel) { textLabel->SetText(text); textLabel->SetVisible(true); }
        SetButtonsVisible(true);
    }

    void DialogBox::Hide()
    {
        visible = false;
        isYesNo = false;
        if (panelImage) panelImage->SetVisible(false);
        if (textLabel)  textLabel->SetVisible(false);
        SetButtonsVisible(false);
        choiceCallback = nullptr;
        dismissCallback = nullptr;
    }

    void DialogBox::DismissIfInfo()
    {
        if (!visible || isYesNo) return;
        visible = false;
        if (panelImage) panelImage->SetVisible(false);
        if (textLabel)  textLabel->SetVisible(false);
        if (dismissCallback) { auto cb = dismissCallback; dismissCallback = nullptr; cb(); }
    }
}
