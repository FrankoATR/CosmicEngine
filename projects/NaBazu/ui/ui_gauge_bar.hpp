#ifndef NABAZU_UI_GAUGE_BAR_HPP
#define NABAZU_UI_GAUGE_BAR_HPP

#include <CosmicEngine/models/ui/ui_element.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // Read-only arcade-style gauge: a caption, a right-aligned readout, and a
    // segmented bar underneath. Purpose-built to replace UISlider for HUD readouts --
    // a slider is an interactive control (drag handle, focus, keyboard steps), so
    // using one as a display widget meant fighting its interaction model and showing
    // a drag handle that means nothing.
    //
    // Registers as UIElementType::Label: like a label it is purely informational, and
    // UIManager::IsMouseHoverAny() deliberately skips Labels, so these gauges never
    // swallow clicks meant for the world.
    class UIGaugeBar : public CosmicEngine::UIElement
    {
    public:
        UIGaugeBar(std::string caption, std::string fontKey, glm::vec2 position, glm::vec2 size,
                    int segmentCount = 18, bool visible = true);

        // Target fill, 0..1. The drawn value eases toward this so changes read as
        // motion instead of popping between frames.
        void SetValue(float normalizedValue);
        // Text shown right-aligned above the bar (e.g. "450 / 600"). Optional.
        void SetReadout(std::string readout);

        void SetBarColor(glm::vec3 color);
        // Blended in as the gauge empties past lowThreshold, and (when enabled)
        // drives a warning pulse. Defaults to a red alert tint.
        void SetLowColor(glm::vec3 color);
        void SetLowThreshold(float threshold);
        void SetPulseWhenLow(bool enabled);
        void SetCaptionColor(glm::vec3 color);

        void update(float deltaTime) override;
        void draw() override;

    private:
        glm::vec3 CurrentBarColor() const;

        std::string caption_;
        std::string fontKey_;
        std::string readout_;

        int segmentCount_;
        float targetValue_;
        float displayedValue_;
        float pulsePhase_;

        glm::vec3 barColor_;
        glm::vec3 lowColor_;
        glm::vec3 captionColor_;
        glm::vec3 trackColor_;
        float lowThreshold_;
        bool pulseWhenLow_;
    };
}

#endif
