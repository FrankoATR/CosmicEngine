#include "ui_text.hpp"
#include "../../../managers/input/input_manager.hpp"
#include "../../../managers/camera/camera_manager.hpp"
#include "../../../managers/resource/resource_manager.hpp"
#include <iostream>
#include <sstream>
#include <vector>

namespace CosmicEngine
{
    namespace
    {
        void Utf8PopBack(std::string &text)
        {
            if (text.empty())
            {
                return;
            }

            std::size_t index = text.size() - 1;
            while (index > 0 && (static_cast<unsigned char>(text[index]) & 0xC0) == 0x80)
            {
                --index;
            }
            text.erase(index);
        }
    }

    UIText::UIText(const std::string &text, const std::string &font, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement *parent, bool multiline, int maxLines, bool leftAlign) : 
        text(text), font(font), UIElement(Position, Size, visible, parent, UIElementType::Label), textColor(glm::vec3(1.0f)), multiline(multiline), maxLines(maxLines), leftAlign(leftAlign)
    {
        auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
        cachedSize.x = Position.x + (Size.x - tmp.x) / 2;
        cachedSize.y = Position.y + (Size.y - tmp.y) / 2;
    }

    void UIText::SetTextScale(float scale)
    {
        textScale = scale;
        cachedText.clear();
    }

    UIText::~UIText()
    {
    }

    void UIText::update(float deltaTime)
    {
        UIElement::update(deltaTime);
    }

    void UIText::draw()
    {
        if (!visible)
        {
            return;
        }

        if(!text.empty() && !font.empty())
        {
            if (text != cachedText)
            {
                cachedText = text;
                auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
                if (!multiline)
                {
                    cachedSize.x = Position.x + (Size.x - tmp.x * textScale) / 2;
                    cachedSize.y = Position.y + (Size.y - tmp.y * textScale) / 2;
                }
                else
                {
                    // For multiline left-aligned text we will position at top-left inside bounds
                    cachedSize.x = Position.x;
                    cachedSize.y = Position.y;
                }
            }

            if (!multiline)
            {
                ResourceManager::GetInstance().RenderText(text, font, glm::vec3(cachedSize.x, cachedSize.y, 1.0f), glm::vec3(textScale), glm::vec3(), glm::vec3(0.0f), textColor, 1.0f, ViewType::UI);
            }
            else
            {
                // Word-wrap into lines that fit within Size.x, starting at Position.x/Position.y
                std::vector<std::string> words;
                {
                    std::istringstream iss(text);
                    std::string w;
                    while (iss >> w) words.push_back(w);
                }
                std::vector<std::string> lines;
                std::string cur;
                // Leave a small inner margin so text never touches the edges.
                float availW = std::max(0.0f, Size.x - 8.0f);
                for (size_t i = 0; i < words.size(); ++i)
                {
                    std::string candidate = cur.empty() ? words[i] : cur + " " + words[i];
                    auto m = ResourceManager::GetInstance().MeasureText(candidate, font, glm::vec3(1.0f));
                    if (m.x <= availW)
                    {
                        cur = candidate;
                    }
                    else
                    {
                        if (!cur.empty()) lines.push_back(cur);
                        cur = words[i];
                        if (lines.size() >= static_cast<size_t>(maxLines)) break;
                    }
                }
                if (!cur.empty() && lines.size() < static_cast<size_t>(maxLines)) lines.push_back(cur);

                // If more text remains than maxLines, truncate the last line and add ellipsis
                bool truncated = false;
                if (lines.size() > static_cast<size_t>(maxLines))
                {
                    lines.resize(maxLines);
                    truncated = true;
                }
                // Measure line height using a sample string and add extra spacing
                auto sample = ResourceManager::GetInstance().MeasureText("Ay", font, glm::vec3(1.0f));
                float lineH = sample.y;
                const float extraLineSpacing = 6.0f; // pixels between lines
                float y = cachedSize.y;
                for (size_t li = 0; li < lines.size() && li < static_cast<size_t>(maxLines); ++li)
                {
                    std::string line = lines[li];
                    if (li == lines.size() - 1 && truncated)
                    {
                        // ensure fits with ellipsis
                        std::string ell = "...";
                        auto m = ResourceManager::GetInstance().MeasureText(line + ell, font, glm::vec3(1.0f));
                        while (m.x > availW && !line.empty())
                        {
                            Utf8PopBack(line);
                            m = ResourceManager::GetInstance().MeasureText(line + ell, font, glm::vec3(1.0f));
                        }
                        line += ell;
                    }

                    float x = leftAlign ? Position.x : Position.x + (Size.x - ResourceManager::GetInstance().MeasureText(line, font, glm::vec3(1.0f)).x) / 2.0f;
                    ResourceManager::GetInstance().RenderText(line, font, glm::vec3(x, y, 1.0f), glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), textColor, 1.0f, ViewType::UI);
                    y += lineH + extraLineSpacing;
                }
            }
        }
        
        UIElement::draw();
    }


    void UIText::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIText::SetFont(const std::string &font)
    {
        this->font = font;
    }

    void UIText::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }

    void UIText::SetMaxLines(int maxLines)
    {
        this->maxLines = maxLines;
    }


}
