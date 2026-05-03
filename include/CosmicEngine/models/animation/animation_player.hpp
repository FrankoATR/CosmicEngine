#ifndef COSMIC_ANIMATION_PLAYER_HPP
#define COSMIC_ANIMATION_PLAYER_HPP

/**
 * @file animation_player.hpp
 * @brief Declares the animation player used to run sprite-sheet animation clips.
 */

#include "animation_clip.hpp"

#include <cstdint>
#include <string>

namespace CosmicEngine
{
    /**
     * @brief Maintains playback state for an animation clip assigned to an object.
     *
     * An AnimationPlayer is created through the AnimationManager and keeps track of
     * which frame of a clip is currently active, the elapsed time within that frame,
     * and whether playback is paused, finished, or looping.  Query GetCurrentRow()
     * and GetCurrentColumn() each frame to render the correct tile from the sheet.
     *
    * @par Example - creating and using an animation player
     * @code
     * // Create a player bound to a registered clip and this object ID
     * animationPlayer = ANM_MN.CreatePlayer("walk_right", GetID());
     * animationPlayer->Play();
     *
     * // Switch clips at runtime (e.g. change direction):
    * animationPlayer->SetClip(ANM_MN.GetClip("walk_left"), true);
     * animationPlayer->Play();
     *
     * // Draw the current frame inside your draw() method:
    * RS_MN.Render2DSpriteFromTextureSheetUnlit(
     *     animationPlayer->GetTextureSheetKey(),
     *     animationPlayer->GetCurrentRow(),
     *     animationPlayer->GetCurrentColumn(),
     *     GetPosition(), GetSize(), GetRotation(),
     *     GetColor(), 1.0f, CosmicEngine::ViewType::Ortho);
     * @endcode
     *
    * @par Example - cleaning up
     * @code
     * if (animationPlayer) { animationPlayer->Destroy(); animationPlayer = nullptr; }
     * @endcode
     */
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
        /**
         * @brief Creates an animation player.
         * @param ownerObjectId Identifier of the owning object, or -1 when unbound.
         */
        explicit AnimationPlayer(std::int64_t ownerObjectId = -1);
        ~AnimationPlayer() = default;

        /**
         * @brief Sets the unique player identifier assigned by the animation manager.
         * @param newId Value provided by the caller.
         */
        void SetId(std::uint64_t newId);
        /**
         * @brief Returns the unique player identifier.
         * @return The unique player identifier.
         */
        std::uint64_t GetId() const;

        /**
         * @brief Sets the identifier of the object that owns the player.
         * @param newOwnerObjectId Value provided by the caller.
         */
        void SetOwnerObjectId(std::int64_t newOwnerObjectId);
        /**
         * @brief Returns the identifier of the object that owns the player.
         * @return The identifier of the object that owns the player.
         */
        std::int64_t GetOwnerObjectId() const;

        /**
         * @brief Sets the clip played by the player.
         * @param newClip Animation clip to play (must outlive the player).
         * @param restart When true the playback time is reset to the start of the clip.
         */
        void SetClip(const AnimationClip *newClip, bool restart = true);
        /** @brief Returns the clip currently assigned to the player. */
        const AnimationClip *GetClip() const;
        /** @brief Returns the name of the current clip, or an empty string when none is assigned. */
        const std::string &GetClipName() const;
        /** @brief Returns the texture-sheet key of the current clip, or an empty string when none is assigned. */
        const std::string &GetTextureSheetKey() const;

        /** @brief Advances the playback state of the current clip. */
        void Update(double deltaTime);

        /** @brief Starts or resumes playback. */
        void Play();
        /** @brief Pauses playback. */
        void Pause();
        /** @brief Stops playback without destroying the player. */
        void Stop();
        /** @brief Restarts playback from the first frame. */
        void Restart();
        /** @brief Marks the player for removal from the runtime. */
        void Destroy();

        /**
         * @brief Returns whether the player is currently playing.
         * @return The whether the player is currently playing.
         */
        bool IsPlaying() const;
        /**
         * @brief Returns whether the current clip reached its end.
         * @return The whether the current clip reached its end.
         */
        bool IsFinished() const;
        /**
         * @brief Returns whether the player is still alive in the runtime.
         * @return The whether the player is still alive in the runtime.
         */
        bool IsAlive() const;

        /**
         * @brief Sets the playback speed multiplier.
         * @param newSpeedMultiplier Value provided by the caller.
         */
        void SetSpeedMultiplier(float newSpeedMultiplier);
        /**
         * @brief Returns the playback speed multiplier.
         * @return The playback speed multiplier.
         */
        float GetSpeedMultiplier() const;

        /**
         * @brief Returns the sprite-sheet row of the current frame.
         * @return The sprite-sheet row of the current frame.
         */
        int GetCurrentRow() const;
        /**
         * @brief Returns the sprite-sheet column of the current frame.
         * @return The sprite-sheet column of the current frame.
         */
        int GetCurrentColumn() const;
        /** @brief Returns the current frame, or null when no clip is active. */
        const AnimationFrame *GetCurrentFrame() const;
    };
}

#endif // COSMIC_ANIMATION_PLAYER_HPP
