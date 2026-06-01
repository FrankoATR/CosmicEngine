#include "ui_slider.hpp"

#include "../../../managers/input/input_manager.hpp"
#include "../../../managers/resource/resource_manager.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace CosmicEngine
{
    UISlider::UISlider(const std::string &label,
                       const std::string &font,
                       glm::vec2 Position,
                       glm::vec2 Size,
                       float minValue,
                       float maxValue,
                       float initialValue,
                       bool visible,
                       UIElement *parent)
        : UIElement(Position, Size, visible, parent, UIElementType::Slider),
          label(label), font(font),
          minValue(minValue), maxValue(maxValue),
          value(std::clamp(initialValue, minValue, maxValue)),
          step((maxValue - minValue) / 20.0f),
          dragging(false), focused(false),
          trackColor(glm::vec3(0.18f)),
          fillColor(glm::vec3(0.25f, 0.70f, 1.00f)),
          handleColor(glm::vec3(0.92f, 0.96f, 1.00f)),
          borderColor(glm::vec3(0.6f)),
          focusBorderColor(glm::vec3(0.2f, 0.7f, 1.0f)),
          textColor(glm::vec3(0.9f, 0.95f, 1.0f)),
          valueFormatter(nullptr),
          onValueChanged(nullptr)
    {
    }

    UISlider::~UISlider() {}

    bool UISlider::IsFocusable() const { return true; }

    void UISlider::SetFocused(bool focused)
    {
        this->focused = focused;
        UIElement::SetFocused(focused);
    }

    void UISlider::Activate()
    {
        SetFocused(true);
    }

    float UISlider::Normalize(float v) const
    {
        if (maxValue <= minValue) return 0.0f;
        return std::clamp((v - minValue) / (maxValue - minValue), 0.0f, 1.0f);
    }

    void UISlider::SetValueFromMouse()
    {
        glm::vec2 mouse = InputManager::GetInstance().GetMousePosition_UI();
        float t = 0.0f;
        if (Size.x > 0.0f)
        {
            t = std::clamp((mouse.x - Position.x) / Size.x, 0.0f, 1.0f);
        }
        float newValue = minValue + t * (maxValue - minValue);
        SetValue(newValue, true);
    }

    void UISlider::update(float deltaTime)
    {
        auto &input = InputManager::GetInstance();

        const bool mouseDown = input.IsMouseButtonPressed(0, KeyEventType::KeyDown);
        const bool mouseHeld = input.IsMouseButtonPressed(0, KeyEventType::KeyRelease);
        const bool mouseUp   = input.IsMouseButtonPressed(0, KeyEventType::KeyUp);

        if (mouseDown && MouseHover())
        {
            dragging = true;
            SetFocused(true);
            SetValueFromMouse();
        }
        else if (dragging && mouseHeld)
        {
            SetValueFromMouse();
        }
        if (mouseUp || !mouseHeld)
        {
            dragging = false;
        }

        if (focused)
        {
            if (input.IsKeyPressed(GLFW_KEY_LEFT, KeyEventType::KeyDown))
            {
                SetValue(value - step, true);
            }
            else if (input.IsKeyPressed(GLFW_KEY_RIGHT, KeyEventType::KeyDown))
            {
                SetValue(value + step, true);
            }
        }

        UIElement::update(deltaTime);
    }

    std::string UISlider::FormatValue() const
    {
        if (valueFormatter) return valueFormatter(value);
        std::ostringstream stream;
        stream.setf(std::ios::fixed);
        stream.precision(2);
        stream << value;
        return stream.str();
    }

    void UISlider::draw()
    {
        if (!visible) return;

        auto &rs = ResourceManager::GetInstance();

        const float trackHeight = std::max(8.0f, Size.y * 0.35f);
        const float trackY = Position.y + (Size.y - trackHeight) * 0.5f;
        const glm::vec2 trackMin(Position.x, trackY);
        const glm::vec2 trackMax(Position.x + Size.x, trackY + trackHeight);
        const glm::vec2 trackPivot((trackMin.x + trackMax.x) * 0.5f, (trackMin.y + trackMax.y) * 0.5f);

        rs.RenderRectangle(trackMin, trackMax, trackPivot, glm::vec2(0.0f),
                           trackColor, 0.95f, 1.0f, true, ViewType::UI);

        const float t = Normalize(value);
        const float fillEnd = Position.x + Size.x * t;
        if (fillEnd > Position.x + 1.0f)
        {
            const glm::vec2 fillMax(fillEnd, trackMax.y);
            const glm::vec2 fillPivot((trackMin.x + fillEnd) * 0.5f, (trackMin.y + trackMax.y) * 0.5f);
            rs.RenderRectangle(trackMin, fillMax, fillPivot, glm::vec2(0.0f),
                               fillColor, 1.0f, 1.0f, true, ViewType::UI);
        }

        const float handleWidth = 14.0f;
        const float handleHeight = Size.y;
        const float handleCenterX = Position.x + Size.x * t;
        const glm::vec2 handleMin(handleCenterX - handleWidth * 0.5f, Position.y);
        const glm::vec2 handleMax(handleCenterX + handleWidth * 0.5f, Position.y + handleHeight);
        const glm::vec2 handlePivot(handleCenterX, Position.y + handleHeight * 0.5f);
        rs.RenderRectangle(handleMin, handleMax, handlePivot, glm::vec2(0.0f),
                           handleColor, 1.0f, 1.0f, true, ViewType::UI);

        const glm::vec3 borderC = focused ? focusBorderColor : borderColor;
        rs.RenderRectangle(handleMin, handleMax, handlePivot, glm::vec2(0.0f),
                           borderC, 1.0f, 2.0f, false, ViewType::UI);
        rs.RenderRectangle(trackMin, trackMax, trackPivot, glm::vec2(0.0f),
                           borderC, 1.0f, 1.0f, false, ViewType::UI);

        if (!font.empty())
        {
            const std::string readout = FormatValue();
            if (!readout.empty())
            {
                auto textSize = rs.MeasureText(readout, font, glm::vec3(1.0f));
                const float yPos = Position.y + (Size.y - textSize.y) * 0.5f;
                const float xPos = Position.x + Size.x + 12.0f;
                rs.RenderText(readout, font,
                              glm::vec3(xPos, yPos, 1.0f),
                              glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f),
                              textColor, 1.0f, ViewType::UI);
            }
        }

        UIElement::draw();
    }

    void UISlider::SetValue(float v, bool notify)
    {
        const float clamped = std::clamp(v, minValue, maxValue);
        if (std::fabs(clamped - value) < 1e-6f)
        {
            value = clamped;
            return;
        }
        value = clamped;
        if (notify && onValueChanged) onValueChanged(value);
    }

    float UISlider::GetValue() const { return value; }

    void UISlider::SetRange(float minValue, float maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        value = std::clamp(value, minValue, maxValue);
    }

    void UISlider::SetStep(float step) { this->step = step; }
    void UISlider::SetLabel(const std::string &label) { this->label = label; }
    void UISlider::SetValueFormatter(std::function<std::string(float)> formatter) { valueFormatter = std::move(formatter); }
    void UISlider::SetOnValueChanged(std::function<void(float)> callback) { onValueChanged = std::move(callback); }
    void UISlider::SetTrackColor(glm::vec3 color) { trackColor = color; }
    void UISlider::SetFillColor(glm::vec3 color) { fillColor = color; }
    void UISlider::SetHandleColor(glm::vec3 color) { handleColor = color; }
    void UISlider::SetBorderColor(glm::vec3 color) { borderColor = color; }
    void UISlider::SetFocusBorderColor(glm::vec3 color) { focusBorderColor = color; }
    void UISlider::SetTextColor(glm::vec3 color) { textColor = color; }
}
