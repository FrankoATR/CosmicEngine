#include "audio_manager.hpp"

#include "../../utils/log.hpp"

#include <algorithm>

namespace CosmicEngine
{
    AudioManager::AudioManager()
    {
        RUNTIME_LIFECYCLE("Audio manager", "created");
    }

    AudioManager::~AudioManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Audio manager", "destroyed");
    }

    bool AudioManager::init()
    {
        if (engineInitialized)
            return true;

        ma_result result = ma_engine_init(nullptr, &engine);
        if (result != MA_SUCCESS)
        {
            RUNTIME_WARNING("[AudioManager] Failed to initialize miniaudio engine. res=" << result);
            engineInitialized = false;
            return false;
        }

        engineInitialized = true;

        masterVolume = 1.0f;
        musicMasterVolume = 1.0f;
        sfxMasterVolume = 1.0f;

        listenerPos = glm::vec3(0.0f);
        panRangeUnits = 10.0f;
        sfxVoicesPerSound = 12;

        ma_engine_listener_set_position(&engine, 0, listenerPos.x, listenerPos.y, listenerPos.z);

        RUNTIME_LIFECYCLE("Audio manager", "initialized");
        return true;
    }

    void AudioManager::update()
    {


    }

    float AudioManager::validate_range(float v) const
    {
        return std::clamp(v, 0.0f, 1.0f);
    }

    float AudioManager::ComputeFinalVolume(const SoundData &data) const
    {
        const float category = (data.type == SoundType::Music) ? musicMasterVolume : sfxMasterVolume;
        return validate_range(masterVolume) * validate_range(category) * validate_range(data.volume);
    }

    float AudioManager::ComputeFinalVolumeWithCall(const SoundData &data, float perCall) const
    {
        return ComputeFinalVolume(data) * validate_range(perCall);
    }

    void AudioManager::ApplyVolumesToAll()
    {
        for (auto &[_, data] : sounds)
        {
            if (!data.initialized)
                continue;

            ma_sound_set_volume(&data.sound, ComputeFinalVolume(data));

            if (data.type == SoundType::SFX)
            {
                for (auto &inst : data.pool)
                {
                    if (!inst.initialized)
                        continue;
                    ma_sound_set_volume(&inst.sound, ComputeFinalVolume(data));
                }
            }
        }
    }

    float AudioManager::ComputePanFrom2D(const glm::vec3 &pos) const
    {
        const float denom = (panRangeUnits <= 0.0001f) ? 0.0001f : panRangeUnits;
        const float dx = (pos.x - listenerPos.x);
        return std::clamp(dx / denom, -1.0f, 1.0f);
    }

    void AudioManager::ApplySoundSpace(ma_sound &snd, SoundSpace space, const glm::vec3 &pos)
    {
        switch (space)
        {
        case SoundSpace::Global:
        {
            ma_sound_set_spatialization_enabled(&snd, MA_FALSE);
            ma_sound_set_pan(&snd, 0.0f);
        }
        break;

        case SoundSpace::World2D:
        {
            ma_sound_set_spatialization_enabled(&snd, MA_FALSE);
            ma_sound_set_pan(&snd, ComputePanFrom2D(pos));
            ma_sound_set_position(&snd, pos.x, pos.y, 0.0f);
        }
        break;

        case SoundSpace::World3D:
        {
            ma_sound_set_spatialization_enabled(&snd, MA_TRUE);
            ma_sound_set_position(&snd, pos.x, pos.y, pos.z);
        }
        break;
        }
    }

    ma_sound *AudioManager::AcquirePlayableSound(SoundData &data, bool restartIfSteal)
    {
        if (data.type == SoundType::Music)
            return &data.sound;

        if (data.pool.empty())
            return &data.sound;

        for (auto &inst : data.pool)
        {
            if (!inst.initialized)
                continue;

            if (!ma_sound_is_playing(&inst.sound))
                return &inst.sound;
        }

        if (data.pool[0].initialized)
        {
            if (restartIfSteal)
            {
                ma_sound_stop(&data.pool[0].sound);
                ma_sound_seek_to_pcm_frame(&data.pool[0].sound, 0);
            }
            return &data.pool[0].sound;
        }

        return nullptr;
    }

    void AudioManager::StopSoundData(SoundData &data)
    {
        if (!data.initialized)
            return;

        ma_sound_stop(&data.sound);

        if (data.type == SoundType::SFX)
        {
            for (auto &inst : data.pool)
            {
                if (!inst.initialized)
                    continue;
                ma_sound_stop(&inst.sound);
            }
        }
    }

    void AudioManager::UninitSoundData(SoundData &data)
    {
        if (!data.initialized)
            return;

        for (auto &inst : data.pool)
        {
            if (!inst.initialized)
                continue;
            ma_sound_uninit(&inst.sound);
            inst.initialized = false;
        }
        data.pool.clear();

        ma_sound_uninit(&data.sound);
        data.initialized = false;
    }

    bool AudioManager::Load(const std::string &key, const std::string &filename, SoundType type)
    {
        if (!engineInitialized)
        {
            RUNTIME_WARNING("[AudioManager] Load failed: engine not initialized. Call init() first.");
            return false;
        }

        if (sounds.find(key) != sounds.end())
            return false;

        auto [it, inserted] = sounds.emplace(key, SoundData{});
        SoundData &data = it->second;

        data.type = type;
        data.volume = 1.0f;
        data.initialized = false;
        data.pool.clear();

        ma_uint32 flags = 0;
        if (type == SoundType::SFX)
            flags = MA_SOUND_FLAG_DECODE;
        else
            flags = MA_SOUND_FLAG_STREAM;

        ma_result res = ma_sound_init_from_file(
            &engine,
            filename.c_str(),
            flags,
            nullptr,
            nullptr,
            &data.sound);

        if (res != MA_SUCCESS)
        {
            RUNTIME_WARNING("[AudioManager] Load failed for key='" << key << "' file='" << filename << "' res=" << res);
            sounds.erase(it);
            return false;
        }

        data.initialized = true;

        ma_sound_set_spatialization_enabled(&data.sound, MA_FALSE);
        ma_sound_set_pan(&data.sound, 0.0f);
        ma_sound_set_volume(&data.sound, ComputeFinalVolume(data));

        if (type == SoundType::SFX)
        {
            const ma_uint32 voices = (sfxVoicesPerSound == 0) ? 1 : sfxVoicesPerSound;

            data.pool.clear();
            data.pool.resize(voices);

            ma_uint32 created = 0;

            for (ma_uint32 i = 0; i < voices; ++i)
            {
                SoundInstance &inst = data.pool[i];
                inst.initialized = false;

                ma_result cres = ma_sound_init_copy(
                    &engine,
                    &data.sound,
                    0,
                    nullptr,
                    &inst.sound);

                if (cres != MA_SUCCESS)
                {
                    break;
                }

                inst.initialized = true;

                ma_sound_set_spatialization_enabled(&inst.sound, MA_FALSE);
                ma_sound_set_pan(&inst.sound, 0.0f);
                ma_sound_set_volume(&inst.sound, ComputeFinalVolume(data));

                created++;
            }

            if (created == 0)
            {
                data.pool.clear();
            }
            else if (created < voices)
            {
                data.pool.resize(created);
            }
        }

        return true;
    }

    bool AudioManager::Play(const std::string &key, float volume, bool loop, bool restart)
    {
        return PlayAt(key, glm::vec3(0.0f), SoundSpace::Global, volume, loop, restart);
    }

    bool AudioManager::PlayAt(
        const std::string &key,
        const glm::vec3 &position,
        SoundSpace space,
        float volume,
        bool loop,
        bool restart)
    {
        auto it = sounds.find(key);
        if (it == sounds.end())
            return false;

        SoundData &data = it->second;
        if (!data.initialized)
            return false;

        ma_sound *snd = AcquirePlayableSound(data, true);
        if (snd == nullptr)
            return false;

        const float finalVol = ComputeFinalVolumeWithCall(data, volume);

        ma_sound_set_looping(snd, loop ? MA_TRUE : MA_FALSE);
        ma_sound_set_volume(snd, finalVol);
        ApplySoundSpace(*snd, space, position);

        if (restart)
        {
            ma_sound_stop(snd);
            ma_sound_seek_to_pcm_frame(snd, 0);
        }

        ma_sound_start(snd);
        return true;
    }

    void AudioManager::Stop(const std::string &key)
    {
        auto it = sounds.find(key);
        if (it == sounds.end())
            return;

        StopSoundData(it->second);
    }

    void AudioManager::StopAll()
    {
        for (auto &[_, data] : sounds)
            StopSoundData(data);
    }

    void AudioManager::StopAll(SoundType type)
    {
        for (auto &[_, data] : sounds)
        {
            if (!data.initialized)
                continue;
            if (data.type == type)
                StopSoundData(data);
        }
    }

    void AudioManager::SetVolume(const std::string &key, float volume)
    {
        auto it = sounds.find(key);
        if (it == sounds.end())
            return;

        SoundData &data = it->second;
        if (!data.initialized)
            return;

        data.volume = validate_range(volume);

        ma_sound_set_volume(&data.sound, ComputeFinalVolume(data));

        if (data.type == SoundType::SFX)
        {
            for (auto &inst : data.pool)
            {
                if (!inst.initialized)
                    continue;
                ma_sound_set_volume(&inst.sound, ComputeFinalVolume(data));
            }
        }
    }

    void AudioManager::SetMasterVolume(float volume)
    {
        masterVolume = validate_range(volume);
        ApplyVolumesToAll();
    }

    void AudioManager::SetMusicMasterVolume(float volume)
    {
        musicMasterVolume = validate_range(volume);
        ApplyVolumesToAll();
    }

    void AudioManager::SetSfxMasterVolume(float volume)
    {
        sfxMasterVolume = validate_range(volume);
        ApplyVolumesToAll();
    }

    void AudioManager::SetPosition(const std::string &key, unsigned int milliseconds)
    {
        auto it = sounds.find(key);
        if (it == sounds.end())
            return;

        SoundData &data = it->second;
        if (!data.initialized)
            return;

        const ma_uint32 sr = ma_engine_get_sample_rate(&engine);
        const double seconds = static_cast<double>(milliseconds) / 1000.0;
        const ma_uint64 frame = static_cast<ma_uint64>(seconds * static_cast<double>(sr));

        ma_sound_seek_to_pcm_frame(&data.sound, frame);

        if (data.type == SoundType::SFX)
        {
            for (auto &inst : data.pool)
            {
                if (!inst.initialized)
                    continue;
                ma_sound_seek_to_pcm_frame(&inst.sound, frame);
            }
        }
    }

    void AudioManager::SetListenerPosition(const glm::vec3 &position)
    {
        if (!engineInitialized)
            return;

        listenerPos = position;
        ma_engine_listener_set_position(&engine, 0, listenerPos.x, listenerPos.y, listenerPos.z);
    }

    void AudioManager::SetPanRangeUnits(float units)
    {
        panRangeUnits = (units < 0.0001f) ? 0.0001f : units;
    }

    void AudioManager::SetSfxVoicesPerSound(ma_uint32 voices)
    {
        sfxVoicesPerSound = (voices == 0) ? 1 : voices;
    }

    void AudioManager::Clear()
    {
        for (auto &[_, data] : sounds)
            UninitSoundData(data);

        sounds.clear();
        RUNTIME_LIFECYCLE("Audio manager", "cleared");
    }

    void AudioManager::shutdown()
    {
        if (!engineInitialized)
            return;

        Clear();
        ma_engine_uninit(&engine);
        engineInitialized = false;

        RUNTIME_LIFECYCLE("Audio manager", "shutdown");
    }
}
