#ifndef PUZZLERPG_DEBUG_TEXT_HPP
#define PUZZLERPG_DEBUG_TEXT_HPP

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <string>
#include <utility>
#include <vector>

namespace PuzzleRPG
{
    inline bool &PuzzleDebugLabelsFlag()
    {
        static bool enabled = true;
        return enabled;
    }
    inline bool ArePuzzleDebugLabelsEnabled()        { return PuzzleDebugLabelsFlag(); }
    inline void SetPuzzleDebugLabelsEnabled(bool v)  { PuzzleDebugLabelsFlag() = v; }

    inline void RenderDebugTextBlock(const glm::vec2 &center,
                                     const std::vector<std::pair<std::string, glm::vec3>> &lines,
                                     float scale = 0.16f,
                                     float lineHeight = 6.5f,
                                     float = 56.0f)
    {
        if (!ArePuzzleDebugLabelsEnabled()) return;
        if (lines.empty()) return;

        float blockHeight = static_cast<float>(lines.size()) * lineHeight;
        float startY = center.y - blockHeight * 0.5f + lineHeight * 0.2f;

        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            float approxWidth = static_cast<float>(lines[i].first.size()) * scale * 10.0f;
            RS_MN.RenderText(lines[i].first,
                             "puzzle_font",
                             glm::vec3(center.x - approxWidth * 0.5f,
                                       startY + static_cast<float>(i) * lineHeight,
                                       0.0f),
                             glm::vec3(scale), glm::vec3(0.0f), glm::vec3(0.0f),
                             glm::vec3(1.0f), 1.0f);
        }
    }
}

#endif