#ifndef COSMIC_ANIMATION_PLAYER_HPP
#define COSMIC_ANIMATION_PLAYER_HPP

#include "animation_clip.hpp"

#include <cstdint>
#include <string>

namespace CosmicEngine
{
    class AnimationPlayer
    {
    private:
        std::uint64_t id;
        std::int64_t ownerObjectId;
        const AnimationClip *clip;
        std::size_t currentFrameIndex;
        double elapsedInCurrentFrame;
        float speedMultiplier;
        bool playing;
        bool finished;
        bool alive;

    public:
        explicit AnimationPlayer(std::int64_t ownerObjectId = -1);
        ~AnimationPlayer() = default;

        void SetId(std::uint64_t newId);
        std::uint64_t GetId() const;

        void SetOwnerObjectId(std::int64_t newOwnerObjectId);
        std::int64_t GetOwnerObjectId() const;

        void SetClip(const AnimationClip *newClip, bool restart = true);
        const AnimationClip *GetClip() const;
        const std::string &GetClipName() const;
        const std::string &GetTextureSheetKey() const;

        void Update(double deltaTime);

        void Play();
        void Pause();
        void Stop();
        void Restart();
        void Destroy();

        bool IsPlaying() const;
        bool IsFinished() const;
        bool IsAlive() const;

        void SetSpeedMultiplier(float newSpeedMultiplier);
        float GetSpeedMultiplier() const;

        int GetCurrentRow() const;
        int GetCurrentColumn() const;
        const AnimationFrame *GetCurrentFrame() const;
    };
}

#endif // COSMIC_ANIMATION_PLAYER_HPP