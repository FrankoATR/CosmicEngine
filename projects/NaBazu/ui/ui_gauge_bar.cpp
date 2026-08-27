#include "ui_gauge_bar.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
    constexpr float kCaptionRowHeight = 26.0f;
    constexpr float kCaptionTextScale = 0.62f;
    constexpr float kSegmentGap = 3.0f;
    constexpr float kValueEaseRate = 9.0f;
}

namespace NaBazu
{
    UIGaugeBar::UIGaugeBar(std::string caption, std::string fontKey, glm::vec2 position, glm::vec2 size,
                            int segmentCount, bool visible)
        : UIElement(position, size, visible, nullptr, CosmicEngine::UIElementType::Label),
          caption_(std::move(caption)),
          fontKey_(std::move(fontKey)),
          readout_(),
          segmentCount_(std::max(1, segmentCount)),
          targetValue_(1.0f),
          displayedValue_(1.0f),
          pulsePhase_(0.0f),
          barColor_(0.3f, 0.8f, 1.0f),
          lowColor_(1.0f, 0.25f, 0.2f),
          captionColor_(0.75f, 0.82f, 0.9f),
          trackColor_(0.10f, 0.12f, 0.16f),
          lowThreshold_(0.3f),
          pulseWhenLow_(false)
    {
    }

    void UIGaugeBar::SetValue(float normalizedValue)
    {
        targetValue_ = glm::clamp(normalizedValue, 0.0f, 1.0f);
    }

    void UIGaugeBar::SetReadout(std::string readout)
    {
        readout_ = std::move(readout);
    }

    void UIGaugeBar::SetBarColor(glm::vec3 color) { barColor_ = color; }
    void UIGaugeBar::SetLowColor(glm::vec3 color) { lowColor_ = color; }
    void UIGaugeBar::SetLowThreshold(float threshold) { lowThreshold_ = glm::clamp(threshold, 0.0f, 1.0f); }
    void UIGaugeBar::SetPulseWhenLow(bool enabled) { pulseWhenLow_ = enabled; }
    void UIGaugeBar::SetCaptionColor(glm::vec3 color) { captionColor_ = color; }

    void UIGaugeBar::update(float deltaTime)
    {
        const float alpha = glm::clamp(kValueEaseRate * deltaTime, 0.0f, 1.0f);
        displayedValue_ = glm::mix(displayedValue_, targetValue_, alpha);
        pulsePhase_ += deltaTime;

        UIElement::update(deltaTime);
    }

    glm::vec3 UIGaugeBar::CurrentBarColor() const
    {
        if (lowThreshold_ <= 0.0f)
        {
            return barColor_;
        }

        // Fully barColor_ above the threshold, ramping to lowColor_ as it empties.
        const float t = 1.0f - glm::clamp(displayedValue_ / lowThreshold_, 0.0f, 1.0f);
        return glm::mix(barColor_, lowColor_, t);
    }

    void UIGaugeBar::draw()
    {
        if (!visible)
        {
            return;
        }

        auto &rs = CosmicEngine::ResourceManager::GetInstance();

        const bool hasCaptionRow = !caption_.empty() || !readout_.empty();
        const float barTop = Position.y + (hasCaptionRow ? kCaptionRowHeight : 0.0f);
        const float barBottom = Position.y + Size.y;
        const float barHeight = std::max(6.0f, barBottom - barTop);

        float alertBoost = 0.0f;
        if (pulseWhenLow_ && displayedValue_ <= lowThreshold_)
        {
            alertBoost = 0.5f + 0.5f * std::sin(pulsePhase_ * 7.0f);
        }

        // Track behind the segments.
        const glm::vec2 trackMin(Position.x, barTop);
        const glm::vec2 trackMax(Position.x + Size.x, barTop + barHeight);
        const glm::vec2 trackPivot((trackMin.x + trackMax.x) * 0.5f, (trackMin.y + trackMax.y) * 0.5f);
        rs.RenderRectangle(trackMin, trackMax, trackPivot, glm::vec2(0.0f),
                            trackColor_, 0.8f, 1.0f, true, CosmicEngine::ViewType::UI);

        // Segmented fill: discrete blocks read as an arcade power meter and make the
        // remaining amount countable at a glance, unlike a continuous bar.
        const float segmentStride = Size.x / static_cast<float>(segmentCount_);
        const float segmentWidth = std::max(2.0f, segmentStride - kSegmentGap);
        const float litExact = displayedValue_ * static_cast<float>(segmentCount_);
        const int litCount = static_cast<int>(std::ceil(litExact - 0.0001f));
        const glm::vec3 barColor = CurrentBarColor();

        for (int i = 0; i < litCount && i < segmentCount_; ++i)
        {
            const float segMinX = Position.x + static_cast<float>(i) * segmentStride;
            const glm::vec2 segMin(segMinX, barTop + 2.0f);
            const glm::vec2 segMax(segMinX + segmentWidth, barTop + barHeight - 2.0f);
            const glm::vec2 segPivot((segMin.x + segMax.x) * 0.5f, (segMin.y + segMax.y) * 0.5f);

            // Leading segment is brightened so the "edge" of the value is easy to
            // track as it moves; partial leading segment also fades by its fraction.
            const bool isLeading = (i == litCount - 1);
            const float leadingFraction = isLeading ? glm::clamp(litExact - static_cast<float>(i), 0.15f, 1.0f) : 1.0f;
            glm::vec3 color = isLeading ? glm::mix(barColor, glm::vec3(1.0f), 0.45f) : barColor;
            color = glm::mix(color, glm::vec3(1.0f, 0.9f, 0.9f), alertBoost * 0.5f);

            rs.RenderRectangle(segMin, segMax, segPivot, glm::vec2(0.0f),
                                color, 0.55f + 0.45f * leadingFraction, 1.0f, true, CosmicEngine::ViewType::UI);
        }

        // Outline last so it sits on top of the fill.
        const glm::vec3 outlineColor = glm::mix(glm::vec3(0.35f, 0.40f, 0.48f), lowColor_, alertBoost);
        rs.RenderRectangle(trackMin, trackMax, trackPivot, glm::vec2(0.0f),
                            outlineColor, 0.9f, 1.5f, false, CosmicEngine::ViewType::UI);

        if (!hasCaptionRow)
        {
            UIElement::draw();
            return;
        }

        const glm::vec3 textScale(kCaptionTextScale);

        if (!caption_.empty())
        {
            rs.RenderText(caption_, fontKey_, glm::vec3(Position.x, Position.y, 1.0f),
                           textScale, glm::vec3(0.0f), glm::vec3(0.0f),
                           captionColor_, 1.0f, CosmicEngine::ViewType::UI);
        }

        if (!readout_.empty())
        {
            const glm::vec2 textSize = rs.MeasureText(readout_, fontKey_, textScale);
            const float readoutX = Position.x + Size.x - textSize.x;
            rs.RenderText(readout_, fontKey_, glm::vec3(readoutX, Position.y, 1.0f),
                           textScale, glm::vec3(0.0f), glm::vec3(0.0f),
                           glm::mix(barColor, glm::vec3(1.0f), 0.35f), 1.0f, CosmicEngine::ViewType::UI);
        }

        UIElement::draw();
    }
}
