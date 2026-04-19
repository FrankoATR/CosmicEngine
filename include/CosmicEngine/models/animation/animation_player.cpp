/**
 * @file animation_player.cpp
 * @brief Implements the runtime animation player used by the engine.
 */

#include "animation_player.hpp"

#include <algorithm>

namespace CosmicEngine
{
    namespace
    {
        const std::string kEmptyString;
    }

    AnimationPlayer::AnimationPlayer(std::int64_t ownerObjectId)
        : id(0),
          ownerObjectId(ownerObjectId),
          clip(nullptr),
          currentFrameIndex(0),
          elapsedInCurrentFrame(0.0),
          speedMultiplier(1.0f),
          playing(false),
          finished(false),
          alive(true)
    {
    }

    void AnimationPlayer::SetId(std::uint64_t newId)
    {
        id = newId;
    }

    std::uint64_t AnimationPlayer::GetId() const
    {
        return id;
    }

    void AnimationPlayer::SetOwnerObjectId(std::int64_t newOwnerObjectId)
    {
        ownerObjectId = newOwnerObjectId;
    }

    std::int64_t AnimationPlayer::GetOwnerObjectId() const
    {
        return ownerObjectId;
    }

    void AnimationPlayer::SetClip(const AnimationClip *newClip, bool restart)
    {
        clip = newClip;

        if (restart)
        {
            Restart();
        }
        else
        {
            finished = false;
        }
    }

    const AnimationClip *AnimationPlayer::GetClip() const
    {
        return clip;
    }

    const std::string &AnimationPlayer::GetClipName() const
    {
        return clip ? clip->GetName() : kEmptyString;
    }

    const std::string &AnimationPlayer::GetTextureSheetKey() const
    {
        return clip ? clip->GetTextureSheetKey() : kEmptyString;
    }

    void AnimationPlayer::Update(double deltaTime)
    {
        if (!alive || !playing || finished || !clip || !clip->IsValid())
        {
            return;
        }

        const std::vector<AnimationFrame> &frames = clip->GetFrames();
        if (frames.empty())
        {
            finished = true;
            playing = false;
            return;
        }

        elapsedInCurrentFrame += std::max(0.0, deltaTime) * std::max(0.0f, speedMultiplier);

        // Consume as many frame durations as necessary so playback remains stable after long frames.
        while (alive && playing && !finished)
        {
            const AnimationFrame *frame = GetCurrentFrame();
            if (!frame)
            {
                finished = true;
                playing = false;
                break;
            }

            double frameDuration = std::max(0.0001, frame->duration);
            if (elapsedInCurrentFrame < frameDuration)
            {
                break;
            }

            elapsedInCurrentFrame -= frameDuration;

            if (currentFrameIndex + 1 < frames.size())
            {
                ++currentFrameIndex;
            }
            else if (clip->IsLoop())
            {
                currentFrameIndex = 0;
            }
            else
            {
                currentFrameIndex = frames.size() - 1;
                finished = true;
                playing = false;
            }
        }
    }

    void AnimationPlayer::Play()
    {
        if (!clip || !clip->IsValid())
        {
            return;
        }

        if (finished)
        {
            Restart();
            return;
        }

        playing = true;
    }

    void AnimationPlayer::Pause()
    {
        playing = false;
    }

    void AnimationPlayer::Stop()
    {
        playing = false;
        currentFrameIndex = 0;
        elapsedInCurrentFrame = 0.0;
        finished = false;
    }

    void AnimationPlayer::Restart()
    {
        currentFrameIndex = 0;
        elapsedInCurrentFrame = 0.0;
        finished = false;
        playing = clip && clip->IsValid();
    }

    void AnimationPlayer::Destroy()
    {
        alive = false;
    }

    bool AnimationPlayer::IsPlaying() const
    {
        return playing;
    }

    bool AnimationPlayer::IsFinished() const
    {
        return finished;
    }

    bool AnimationPlayer::IsAlive() const
    {
        return alive;
    }

    void AnimationPlayer::SetSpeedMultiplier(float newSpeedMultiplier)
    {
        speedMultiplier = std::max(0.0f, newSpeedMultiplier);
    }

    float AnimationPlayer::GetSpeedMultiplier() const
    {
        return speedMultiplier;
    }

    int AnimationPlayer::GetCurrentRow() const
    {
        const AnimationFrame *frame = GetCurrentFrame();
        return frame ? frame->row : 0;
    }

    int AnimationPlayer::GetCurrentColumn() const
    {
        const AnimationFrame *frame = GetCurrentFrame();
        return frame ? frame->column : 0;
    }

    const AnimationFrame *AnimationPlayer::GetCurrentFrame() const
    {
        return clip ? clip->GetFrame(currentFrameIndex) : nullptr;
    }
}