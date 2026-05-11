#include "MusicManager.hpp"
#include <iostream>

namespace WandEngine
{

    MusicManager::MusicManager() : system(nullptr)
    {

    }

    MusicManager::~MusicManager()
    {
        Shutdown();
    }

    bool MusicManager::Init()
    {
        if (FMOD_System_Create(&system, FMOD_VERSION) != FMOD_OK)
        {
            std::cerr << "Failed to create FMOD system in MusicManager." << std::endl;
            return false;
        }
        if (FMOD_System_Init(system, 512, FMOD_INIT_NORMAL, nullptr) != FMOD_OK)
        {
            std::cerr << "Failed to initialize FMOD system in MusicManager." << std::endl;
            return false;
        }
        return true;
    }

    void MusicManager::Update()
    {
        if (system)
        {
            FMOD_System_Update(system);
        }
    }

    bool MusicManager::Load(const std::string &key, const std::string &filename)
    {
        FMOD_SOUND *music;
        if (FMOD_System_CreateSound(system, filename.c_str(), FMOD_DEFAULT | FMOD_CREATESTREAM, nullptr, &music) != FMOD_OK)
        {
            std::cerr << "Failed to load music: " << filename << std::endl;
            return false;
        }
        musics[key] = music;
        return true;
    }

    bool MusicManager::Play(const std::string &key, float volume, bool loop)
    {
        auto it = musics.find(key);
        if (it == musics.end())
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

    void MusicManager::Stop(const std::string &key)
    {
        auto it = channels.find(key);
        if (it != channels.end() && it->second)
        {
            FMOD_Channel_Stop(it->second);
        }
    }

    void MusicManager::StopAll()
    {
        for (auto &[_, channel] : channels)
        {
            if (channel)
                FMOD_Channel_Stop(channel);
        }
    }

    void MusicManager::SetVolume(const std::string &key, float volume)
    {
        auto it = channels.find(key);
        if (it != channels.end() && it->second)
        {
            FMOD_Channel_SetVolume(it->second, volume);
        }
    }

    void MusicManager::Clear()
    {
        for (auto &[_, music] : musics)
        {
            if (music)
                FMOD_Sound_Release(music);
        }
        musics.clear();
        channels.clear();
    }

    void MusicManager::Shutdown()
    {
        if (system)
        {
            FMOD_System_Close(system);
            FMOD_System_Release(system);
            system = nullptr;
        }
    }

}
