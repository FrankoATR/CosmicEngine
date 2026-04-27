#ifndef COSMIC_ANIMATION_CLIP_HPP
#define COSMIC_ANIMATION_CLIP_HPP

/**
 * @file animation_clip.hpp
 * @brief Declares sprite-sheet animation clip data structures.
 */

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace CosmicEngine
{
    /**
     * @brief Describes a single frame inside a sprite-sheet animation clip.
     */
    struct AnimationFrame
    {
        /** @brief Sprite-sheet row of the frame. */
        int row = 0;
        /** @brief Sprite-sheet column of the frame. */
        int column = 0;
        /** @brief Duration of the frame in seconds. */
        double duration = 0.1;

        /** @brief Serializes the frame to JSON. */
        nlohmann::json ToJson() const;
        /** @brief Deserializes a frame from JSON. */
        static AnimationFrame FromJson(const nlohmann::json &json);
    };

    /**
     * @brief Represents a named sprite-sheet animation clip.
     *
     * An AnimationClip is a lightweight data object that groups an ordered list of
     * AnimationFrame entries, associates them with a texture-sheet resource key,
     * and indicates whether the animation should loop.  Clips are registered
     * with the AnimationManager and then assigned to AnimationPlayer instances.
     *
    * @par Example - registering a looping 4-frame walk animation
     * @code
     * std::vector<CosmicEngine::AnimationFrame> frames = {
     *     {0, 0, 0.14},   // row 0, column 0, 0.14 s
     *     {0, 1, 0.14},
     *     {0, 2, 0.14},
     *     {0, 3, 0.14}
     * };
     * ANM_MN.RegisterClip(
     *     CosmicEngine::AnimationClip("walk_down", "player_sheet", frames, true));
     * @endcode
     */
    class AnimationClip
    {
    private:
        std::string name;
        std::string textureSheetKey;
        std::vector<AnimationFrame> frames;
        bool loop;

    public:
        /** @brief Creates an empty animation clip. */
        AnimationClip();
        /**
         * @brief Creates an animation clip.
         * @param name Clip name.
         * @param textureSheetKey Texture-sheet resource key.
         * @param frames Ordered animation frames.
         * @param loop True when the clip should loop.
         */
        AnimationClip(std::string name, std::string textureSheetKey, std::vector<AnimationFrame> frames, bool loop = true);

        /** @brief Returns the clip name. */
        const std::string &GetName() const;
        /** @brief Returns the texture-sheet resource key used by the clip. */
        const std::string &GetTextureSheetKey() const;
        /** @brief Returns the ordered clip frames. */
        const std::vector<AnimationFrame> &GetFrames() const;
        /**
         * @brief Returns whether the clip loops.
         * @return The whether the clip loops.
         */
        bool IsLoop() const;
        /**
         * @brief Returns whether the clip contains enough data to be played.
         * @return The whether the clip contains enough data to be played.
         */
        bool IsValid() const;
        /** @brief Returns a frame by index or null when the index is invalid. */
        const AnimationFrame *GetFrame(std::size_t index) const;

        /** @brief Serializes the clip to JSON. */
        nlohmann::json ToJson() const;
        /** @brief Deserializes a clip from JSON. */
        static AnimationClip FromJson(const nlohmann::json &json);
    };
}

#endif // COSMIC_ANIMATION_CLIP_HPP
