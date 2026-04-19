/**
 * @file animation_manager.cpp
 * @brief Implements the runtime animation manager used by the engine.
 */

#include "animation_manager.hpp"

#include <algorithm>
#include <iostream>

namespace CosmicEngine
{
    AnimationManager &AnimationManager::GetInstance()
    {
        static AnimationManager instance;
        return instance;
    }

    AnimationManager::AnimationManager()
        : nextPlayerId(1)
    {
        std::cout << "Animation manager created" << std::endl;
    }

    AnimationManager::~AnimationManager()
    {
        shutdown();
        std::cout << "Animation manager destroyed" << std::endl;
    }

    void AnimationManager::init()
    {
        clips.clear();
        players.clear();
        nextPlayerId = 1;
        std::cout << "Animation manager initialized" << std::endl;
    }

    void AnimationManager::shutdown()
    {
    }

    void AnimationManager::update(double deltaTime)
    {
        for (auto it = players.begin(); it != players.end();)
        {
            AnimationPlayer *player = *it;
            if (!player)
            {
                it = players.erase(it);
                continue;
            }

            player->Update(deltaTime);
            if (!player->IsAlive())
            {
                delete player;
                it = players.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool AnimationManager::RegisterClip(const AnimationClip &clip)
    {
        if (!clip.IsValid())
        {
            return false;
        }

        clips[clip.GetName()] = clip;
        return true;
    }

    bool AnimationManager::RemoveClip(const std::string &clipName)
    {
        return clips.erase(clipName) > 0;
    }

    bool AnimationManager::HasClip(const std::string &clipName) const
    {
        return clips.find(clipName) != clips.end();
    }

    const AnimationClip *AnimationManager::GetClip(const std::string &clipName) const
    {
        auto it = clips.find(clipName);
        return it != clips.end() ? &it->second : nullptr;
    }

    AnimationPlayer *AnimationManager::CreatePlayer(const std::string &clipName, std::int64_t ownerObjectId)
    {
        AnimationPlayer *player = new AnimationPlayer(ownerObjectId);

        if (!clipName.empty())
        {
            player->SetClip(GetClip(clipName), true);
        }

        AddPlayer(player);
        return player;
    }

    void AnimationManager::AddPlayer(AnimationPlayer *player)
    {
        if (!player)
        {
            return;
        }

        if (player->GetId() == 0)
        {
            player->SetId(nextPlayerId++);
        }

        players.push_back(player);
    }

    void AnimationManager::RemovePlayer(std::uint64_t playerId)
    {
        for (AnimationPlayer *player : players)
        {
            if (player && player->GetId() == playerId)
            {
                player->Destroy();
                return;
            }
        }
    }

    AnimationPlayer *AnimationManager::GetPlayer(std::uint64_t playerId) const
    {
        for (AnimationPlayer *player : players)
        {
            if (player && player->GetId() == playerId)
            {
                return player;
            }
        }

        return nullptr;
    }

    nlohmann::json AnimationManager::ExportClips() const
    {
        nlohmann::json clipsJson = nlohmann::json::array();
        for (const auto &pair : clips)
        {
            clipsJson.push_back(pair.second.ToJson());
        }
        return clipsJson;
    }

    void AnimationManager::ImportClips(const nlohmann::json &clipsJson, bool append)
    {
        if (!append)
        {
            clips.clear();
        }

        if (!clipsJson.is_array())
        {
            return;
        }

        for (const nlohmann::json &clipJson : clipsJson)
        {
            AnimationClip clip = AnimationClip::FromJson(clipJson);
            if (clip.IsValid())
            {
                clips[clip.GetName()] = std::move(clip);
            }
        }
    }

    void AnimationManager::Clear()
    {
        while (!players.empty())
        {
            delete players.back();
            players.pop_back();
        }

        clips.clear();
        nextPlayerId = 1;
        std::cout << "Animation manager cleared" << std::endl;
    }
}