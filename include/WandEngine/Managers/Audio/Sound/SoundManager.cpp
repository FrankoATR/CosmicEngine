#include "SoundManager.hpp"
#include <iostream>

namespace WandEngine
{

    SoundManager::SoundManager() : system(nullptr)
    {
    }

    SoundManager::~SoundManager()
    {
        Shutdown();
    }

    bool SoundManager::Init()
    {
        if (FMOD_System_Create(&system, FMOD_VERSION) != FMOD_OK)
        {
            std::cerr << "Failed to create FMOD system in soundManager." << std::endl;
            return false;
        }
        if (FMOD_System_Init(system, 512, FMOD_INIT_NORMAL, nullptr) != FMOD_OK)
        {
            std::cerr << "Failed to initialize FMOD system in soundManager." << std::endl;
            return false;
        }
        return true;
    }

    void SoundManager::Update()
    {
        if (system)
        {
            FMOD_System_Update(system);
        }
    }

    bool SoundManager::Load(const std::string &key, const std::string &filename)
    {
        FMOD_SOUND *sound;
        if (FMOD_System_CreateSound(system, filename.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK)
        {
            std::cerr << "Failed to load sound: " << filename << std::endl;
            return false;
        }
        sounds[key] = sound;
        return true;
    }

    bool SoundManager::Play(const std::string &key, float volume, bool loop)
    {
        auto it = sounds.find(key);
        if (it == sounds.end())
            return false;

        FMOD_CHANNEL *channel = nullptr;
        FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        FMOD_Sound_SetMode(it->second, mode);

        if (FMOD_System_PlaySound(system, it->second, nullptr, false, &channel) == FMOD_OK)
        {
            FMOD_Channel_SetVolume(channel, volume);
            channels[key] = channel;
            return true;
        }
        return false;
    }

    void SoundManager::Stop(const std::string &key)
    {
        auto it = channels.find(key);
        if (it != channels.end() && it->second)
        {
            FMOD_Channel_Stop(it->second);
        }
    }

    void SoundManager::StopAll()
    {
        for (auto &[_, channel] : channels)
        {
            if (channel)
                FMOD_Channel_Stop(channel);
        }
    }

    void SoundManager::SetVolume(const std::string &key, float volume)
    {
        auto it = channels.find(key);
        if (it != channels.end() && it->second)
        {
            FMOD_Channel_SetVolume(it->second, volume);
        }
    }

    void SoundManager::Clear()
    {
        for (auto &[_, sound] : sounds)
        {
            if (sound)
                FMOD_Sound_Release(sound);
        }
        sounds.clear();
        channels.clear();
    }

    void SoundManager::Shutdown()
    {
        if (system)
        {
            FMOD_System_Close(system);
            FMOD_System_Release(system);
            system = nullptr;
        }
    }

}