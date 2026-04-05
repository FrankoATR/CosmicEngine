#ifndef COSMIC_ANIMATION_MANAGER_HPP
#define COSMIC_ANIMATION_MANAGER_HPP

#include "../../models/animation/animation_clip.hpp"
#include "../../models/animation/animation_player.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace CosmicEngine
{
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
        static AnimationManager &GetInstance();

        void init();
        void shutdown();
        void update(double deltaTime);

        bool RegisterClip(const AnimationClip &clip);
        bool RemoveClip(const std::string &clipName);
        bool HasClip(const std::string &clipName) const;
        const AnimationClip *GetClip(const std::string &clipName) const;

        AnimationPlayer *CreatePlayer(const std::string &clipName = std::string(), std::int64_t ownerObjectId = -1);
        void AddPlayer(AnimationPlayer *player);
        void RemovePlayer(std::uint64_t playerId);
        AnimationPlayer *GetPlayer(std::uint64_t playerId) const;

        nlohmann::json ExportClips() const;
        void ImportClips(const nlohmann::json &clipsJson, bool append = false);

        void Clear();
    };
}

#endif // COSMIC_ANIMATION_MANAGER_HPP