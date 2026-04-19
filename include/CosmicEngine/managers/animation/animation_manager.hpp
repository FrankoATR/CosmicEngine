#ifndef COSMIC_ANIMATION_MANAGER_HPP
#define COSMIC_ANIMATION_MANAGER_HPP

/**
 * @file animation_manager.hpp
 * @brief Declares the animation manager used by the engine.
 */

#include "../../models/animation/animation_clip.hpp"
#include "../../models/animation/animation_player.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace CosmicEngine
{
    /**
     * @brief Stores animation clips and owns runtime animation players.
     *
     * AnimationManager acts as the central registry for AnimationClip definitions
     * and the factory/owner of AnimationPlayer instances.  Register your clips
     * once (usually in Scene::loadResources), then call CreatePlayer() to create
     * a player that you can attach to an object for runtime playback.
     *
     * @par Example — registering clips and creating a player
     * @code
     * // Inside Scene::loadResources():
     * RS_MN.LoadTextureSheet("player_sheet", "assets/textures/player2.png", 4, 4, 0);
     *
     * std::vector<CosmicEngine::AnimationFrame> frames = {
     *     {0, 0, 0.14}, {0, 1, 0.14}, {0, 2, 0.14}, {0, 3, 0.14}
     * };
     * ANM_MN.RegisterClip(CosmicEngine::AnimationClip("walk_down", "player_sheet", frames, true));
     *
     * // Inside an Object init:
     * animPlayer = ANM_MN.CreatePlayer("walk_down", GetID());
     * if (animPlayer) animPlayer->Play();
     * @endcode
     */
    class AnimationManager
    {
    private:
        std::unordered_map<std::string, AnimationClip> clips;
        std::vector<AnimationPlayer *> players;
        std::uint64_t nextPlayerId;

        AnimationManager();
        ~AnimationManager();
        AnimationManager(const AnimationManager &) = delete;
        AnimationManager &operator=(const AnimationManager &) = delete;

    public:
        /** @brief Returns the singleton instance of the animation manager. */
        static AnimationManager &GetInstance();

        /** @brief Initializes the animation manager state. */
        void init();
        /** @brief Shuts the animation manager down and releases owned players. */
        void shutdown();
        /** @brief Updates all registered animation players. */
        void update(double deltaTime);

        /** @brief Registers an animation clip by name. */
        bool RegisterClip(const AnimationClip &clip);
        /** @brief Removes an animation clip by name. */
        bool RemoveClip(const std::string &clipName);
        /** @brief Returns whether a clip with the provided name exists. */
        bool HasClip(const std::string &clipName) const;
        /** @brief Returns a registered clip by name. */
        const AnimationClip *GetClip(const std::string &clipName) const;

        /** @brief Creates a new animation player optionally bound to a clip and owner object. */
        AnimationPlayer *CreatePlayer(const std::string &clipName = std::string(), std::int64_t ownerObjectId = -1);
        /** @brief Adds an externally created animation player to the runtime. */
        void AddPlayer(AnimationPlayer *player);
        /** @brief Removes and deletes an animation player by identifier. */
        void RemovePlayer(std::uint64_t playerId);
        /** @brief Returns an animation player by identifier. */
        AnimationPlayer *GetPlayer(std::uint64_t playerId) const;

        /** @brief Serializes all registered clips to JSON. */
        nlohmann::json ExportClips() const;
        /** @brief Imports clips from JSON, optionally appending to the existing registry. */
        void ImportClips(const nlohmann::json &clipsJson, bool append = false);

        /** @brief Clears all clips and runtime players. */
        void Clear();
    };
}

#endif // COSMIC_ANIMATION_MANAGER_HPP