#ifndef COSMIC_AUDIOMANAGER_HPP
#define COSMIC_AUDIOMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <miniaudio/miniaudio.h>

namespace CosmicEngine
{
    enum class SoundType
    {
        Music,
        SFX
    };

    enum class SoundSpace
    {
        Global,
        World2D,
        World3D
    };

    struct SoundInstance
    {
        ma_sound sound{};
        bool initialized = false;
    };

    struct SoundData
    {
        ma_sound sound{};
        SoundType type{SoundType::SFX};

        float volume = 1.0f;

        bool initialized = false;

        std::vector<SoundInstance> pool;
    };

    class AudioManager
    {
    private:
        ma_engine engine{};
        bool engineInitialized = false;

        std::unordered_map<std::string, SoundData> sounds;

        float masterVolume = 1.0f;
        float musicMasterVolume = 1.0f;
        float sfxMasterVolume = 1.0f;

        ma_uint32 sfxVoicesPerSound = 12;

        glm::vec3 listenerPos{0.0f, 0.0f, 0.0f};

        float panRangeUnits = 10.0f;

        AudioManager();
        ~AudioManager();

        float validate_range(float v) const;
        float ComputeFinalVolume(const SoundData &data) const;
        float ComputeFinalVolumeWithCall(const SoundData &data, float perCall) const;

        void ApplyVolumesToAll();

        ma_sound *AcquirePlayableSound(SoundData &data, bool restartIfSteal);

        void ApplySoundSpace(ma_sound &snd, SoundSpace space, const glm::vec3 &pos);

        float ComputePanFrom2D(const glm::vec3 &pos) const;

        void StopSoundData(SoundData &data);

        void UninitSoundData(SoundData &data);

    public:
        static AudioManager &GetInstance()
        {
            static AudioManager instance;
            return instance;
        }

        bool init();
        void update();

        bool Load(const std::string &key, const std::string &filename, SoundType type);

        bool Play(const std::string &key, float volume = 1.0f, bool loop = false, bool restart = true);

        bool PlayAt(
            const std::string &key,
            const glm::vec3 &position,
            SoundSpace space,
            float volume = 1.0f,
            bool loop = false,
            bool restart = true);

        void Stop(const std::string &key);
        void StopAll();
        void StopAll(SoundType type);

        void SetVolume(const std::string &key, float volume);
        void SetMasterVolume(float volume);
        void SetMusicMasterVolume(float volume);
        void SetSfxMasterVolume(float volume);

        void SetPosition(const std::string &key, unsigned int milliseconds);
        void SetListenerPosition(const glm::vec3 &position);
        void SetPanRangeUnits(float units);
        void SetSfxVoicesPerSound(ma_uint32 voices);

        void Clear();
        void shutdown();
    };
}

#endif // COSMIC_AUDIOMANAGER_HPP
