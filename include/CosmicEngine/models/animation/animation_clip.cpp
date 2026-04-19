/**
 * @file animation_clip.cpp
 * @brief Implements sprite-sheet animation clip serialization and validation helpers.
 */

#include "animation_clip.hpp"

namespace CosmicEngine
{
    nlohmann::json AnimationFrame::ToJson() const
    {
        return {
            {"row", row},
            {"column", column},
            {"duration", duration}
        };
    }

    AnimationFrame AnimationFrame::FromJson(const nlohmann::json &json)
    {
        AnimationFrame frame;
        frame.row = json.value("row", 0);
        frame.column = json.value("column", 0);
        frame.duration = json.value("duration", 0.1);
        return frame;
    }

    AnimationClip::AnimationClip()
        : loop(true)
    {
    }

    AnimationClip::AnimationClip(std::string name, std::string textureSheetKey, std::vector<AnimationFrame> frames, bool loop)
        : name(std::move(name)),
          textureSheetKey(std::move(textureSheetKey)),
          frames(std::move(frames)),
          loop(loop)
    {
    }

    const std::string &AnimationClip::GetName() const
    {
        return name;
    }

    const std::string &AnimationClip::GetTextureSheetKey() const
    {
        return textureSheetKey;
    }

    const std::vector<AnimationFrame> &AnimationClip::GetFrames() const
    {
        return frames;
    }

    bool AnimationClip::IsLoop() const
    {
        return loop;
    }

    bool AnimationClip::IsValid() const
    {
        return !name.empty() && !textureSheetKey.empty() && !frames.empty();
    }

    const AnimationFrame *AnimationClip::GetFrame(std::size_t index) const
    {
        if (index >= frames.size())
        {
            return nullptr;
        }

        return &frames[index];
    }

    nlohmann::json AnimationClip::ToJson() const
    {
        nlohmann::json frameArray = nlohmann::json::array();
        for (const AnimationFrame &frame : frames)
        {
            frameArray.push_back(frame.ToJson());
        }

        return {
            {"name", name},
            {"textureSheetKey", textureSheetKey},
            {"loop", loop},
            {"frames", frameArray}
        };
    }

    AnimationClip AnimationClip::FromJson(const nlohmann::json &json)
    {
        std::vector<AnimationFrame> parsedFrames;
        if (json.contains("frames") && json["frames"].is_array())
        {
            for (const nlohmann::json &frameJson : json["frames"])
            {
                parsedFrames.push_back(AnimationFrame::FromJson(frameJson));
            }
        }

        return AnimationClip(
            json.value("name", std::string()),
            json.value("textureSheetKey", std::string()),
            std::move(parsedFrames),
            json.value("loop", true));
    }
}