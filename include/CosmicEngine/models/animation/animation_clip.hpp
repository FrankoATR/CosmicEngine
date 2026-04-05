#ifndef COSMIC_ANIMATION_CLIP_HPP
#define COSMIC_ANIMATION_CLIP_HPP

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace CosmicEngine
{
    struct AnimationFrame
    {
        int row = 0;
        int column = 0;
        double duration = 0.1;

        nlohmann::json ToJson() const;
        static AnimationFrame FromJson(const nlohmann::json &json);
    };

    class AnimationClip
    {
    private:
        std::string name;
        std::string textureSheetKey;
        std::vector<AnimationFrame> frames;
        bool loop;

    public:
        AnimationClip();
        AnimationClip(std::string name, std::string textureSheetKey, std::vector<AnimationFrame> frames, bool loop = true);

        const std::string &GetName() const;
        const std::string &GetTextureSheetKey() const;
        const std::vector<AnimationFrame> &GetFrames() const;
        bool IsLoop() const;
        bool IsValid() const;
        const AnimationFrame *GetFrame(std::size_t index) const;

        nlohmann::json ToJson() const;
        static AnimationClip FromJson(const nlohmann::json &json);
    };
}

#endif // COSMIC_ANIMATION_CLIP_HPP