#ifndef COSMIC_UISLIDER_HPP
#define COSMIC_UISLIDER_HPP

/**
 * @file ui_slider.hpp
 * @brief Declares the retained UI horizontal slider element.
 */

#include <glm/glm.hpp>
#include "../ui_element.hpp"

#include <functional>
#include <string>

namespace CosmicEngine
{
    /**
     * @brief UI element that exposes a configurable value through a draggable horizontal track.
     *
     * UISlider provides a horizontal slider with mouse drag, keyboard arrow nudging when focused,
     * and an optional textual readout. It reports value changes through an std::function callback
     * so callers can apply them live (volume, sensitivity, etc.).
     */
    class UISlider : public UIElement
    {
    public:
        UISlider(const std::string &label,
                 const std::string &font,
                 glm::vec2 Position,
                 glm::vec2 Size,
                 float minValue,
                 float maxValue,
                 float initialValue,
                 bool visible = true,
                 UIElement *parent = nullptr);
        virtual ~UISlider();

        void update(float deltaTime) override;
        void draw() override;
        bool IsFocusable() const override;
        void SetFocused(bool focused) override;
        void Activate() override;

        void SetValue(float value, bool notify = true);
        float GetValue() const;

        void SetRange(float minValue, float maxValue);
        void SetStep(float step);

        void SetLabel(const std::string &label);
        void SetValueFormatter(std::function<std::string(float)> formatter);
        void SetOnValueChanged(std::function<void(float)> callback);

        void SetTrackColor(glm::vec3 color);
        void SetFillColor(glm::vec3 color);
        void SetHandleColor(glm::vec3 color);
        void SetBorderColor(glm::vec3 color);
        void SetFocusBorderColor(glm::vec3 color);
        void SetTextColor(glm::vec3 color);

    private:
        std::string label;
        std::string font;
        float minValue;
        float maxValue;
        float value;
        float step;
        bool dragging;
        bool focused;

        glm::vec3 trackColor;
        glm::vec3 fillColor;
        glm::vec3 handleColor;
        glm::vec3 borderColor;
        glm::vec3 focusBorderColor;
        glm::vec3 textColor;

        std::function<std::string(float)> valueFormatter;
        std::function<void(float)> onValueChanged;

        float Normalize(float v) const;
        void SetValueFromMouse();
        std::string FormatValue() const;
    };
}

#endif // COSMIC_UISLIDER_HPP
