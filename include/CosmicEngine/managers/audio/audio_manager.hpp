#ifndef COSMIC_AUDIOMANAGER_HPP
#define COSMIC_AUDIOMANAGER_HPP

/**
 * @file audio_manager.hpp
 * @brief Declares the audio manager and audio data structures used by the engine.
 */

#include <unordered_map>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <miniaudio/miniaudio.h>

namespace CosmicEngine
{
    /**
     * @brief Categorizes an audio asset according to its playback role.
     */
    enum class SoundType
    {
        /** @brief Long-running streamed or looped music track. */
        Music,
        /** @brief Sound effect that can be played once or overlapped through pooled voices. */
        SFX
    };

    /**
     * @brief Describes how an audio playback request should be spatialized.
     */
    enum class SoundSpace
    {
        /** @brief Non-spatialized sound played equally for the listener. */
        Global,
        /** @brief 2D spatial sound using listener-relative panning. */
        World2D,
        /** @brief Fully spatialized 3D sound. */
        World3D
    };

    /**
     * @brief Stores a single playable sound instance used by pooled sound effects.
     */
    struct SoundInstance
    {
        /** @brief Miniaudio sound object. */
        ma_sound sound{};
        /** @brief True when the sound object has been initialized. */
        bool initialized = false;
    };

    /**
     * @brief Stores a loaded sound asset and its shared runtime metadata.
     */
    struct SoundData
    {
        /** @brief Primary miniaudio sound object associated with the asset. */
        ma_sound sound{};
        /** @brief Logical sound category. */
        SoundType type{SoundType::SFX};

        /** @brief Per-asset volume multiplier. */
        float volume = 1.0f;

        /** @brief True when the sound asset is fully initialized. */
        bool initialized = false;

        /** @brief Reusable sound instances for overlapping SFX playback. */
        std::vector<SoundInstance> pool;
    };

    /**
     * @brief Manages loading, playback, mixing, and spatialization of engine audio.
     *
     * AudioManager wraps miniaudio and provides a high-level API for music tracks
     * and pooled sound effects with spatial panning (2D or 3D).  Sounds are loaded
     * by key and played via AUD_MN.
     *
     * @par Example — loading and playing audio
     * @code
     * AUD_MN.LoadSound("bg_music", "assets/music/theme.ogg", CosmicEngine::SoundType::Music);
     * AUD_MN.Play("bg_music");
     *
     * AUD_MN.LoadSound("explosion", "assets/music/boom.wav", CosmicEngine::SoundType::SFX);
     * AUD_MN.Play("explosion");
     * @endcode
     */
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
        /** @brief Returns the singleton instance of the audio manager. */
        static AudioManager &GetInstance()
        {
            static AudioManager instance;
            return instance;
        }

        /**
         * @brief Initializes the underlying miniaudio engine.
         * @return True when the audio engine is ready to use.
         */
        bool init();
        /** @brief Updates audio runtime state. */
        void update();

        /**
         * @brief Loads an audio asset into the manager.
         * @param key Logical key used to identify the asset.
         * @param filename Source audio file path.
         * @param type Logical sound category.
         * @return True when the asset was loaded successfully.
         */
        bool Load(const std::string &key, const std::string &filename, SoundType type);

        /**
         * @brief Plays a non-spatial sound.
         * @param key Logical sound key.
         * @param volume Per-call volume multiplier.
         * @param loop True to loop playback.
         * @param restart True to restart an already playing sound.
         * @return True when playback started successfully.
         */
        bool Play(const std::string &key, float volume = 1.0f, bool loop = false, bool restart = true);

        /**
         * @brief Plays a sound using explicit spatial information.
         * @param key Logical sound key.
         * @param position World position of the sound source.
         * @param space Spatialization mode.
         * @param volume Per-call volume multiplier.
         * @param loop True to loop playback.
         * @param restart True to restart an already playing sound.
         * @return True when playback started successfully.
         */
        bool PlayAt(
            const std::string &key,
            const glm::vec3 &position,
            SoundSpace space,
            float volume = 1.0f,
            bool loop = false,
            bool restart = true);

        /**
         * @brief Stops a sound by key.
         * @param key Logical sound key.
         */
        void Stop(const std::string &key);
        /** @brief Stops every loaded sound. */
        void StopAll();
        /**
         * @brief Stops every loaded sound of the requested type.
         * @param type Target sound category.
         */
        void StopAll(SoundType type);

        /**
         * @brief Sets the base volume of a loaded sound.
         * @param key Logical sound key.
         * @param volume New volume multiplier.
         */
        void SetVolume(const std::string &key, float volume);
        /**
         * @brief Sets the global master volume.
         * @param volume New master volume multiplier.
         */
        void SetMasterVolume(float volume);
        /** @brief Returns the global master volume multiplier. */
        float GetMasterVolume() const;
        /**
         * @brief Sets the master volume applied to music sounds.
         * @param volume New music volume multiplier.
         */
        void SetMusicMasterVolume(float volume);
        /** @brief Returns the master volume applied to music sounds. */
        float GetMusicMasterVolume() const;
        /**
         * @brief Sets the master volume applied to sound effects.
         * @param volume New sound-effects volume multiplier.
         */
        void SetSfxMasterVolume(float volume);
        /** @brief Returns the master volume applied to sound effects. */
        float GetSfxMasterVolume() const;

        /**
         * @brief Seeks a sound to a playback position in milliseconds.
         * @param key Logical sound key.
         * @param milliseconds Playback position in milliseconds.
         */
        void SetPosition(const std::string &key, unsigned int milliseconds);
        /**
         * @brief Returns the current playback position of a sound in milliseconds.
         * @param key Logical sound key.
         * @return Current playback position in milliseconds, or 0 when unavailable.
         */
        unsigned int GetPosition(const std::string &key) const;
        /**
         * @brief Sets the listener position used by spatial playback.
         * @param position Listener world position.
         */
        void SetListenerPosition(const glm::vec3 &position);
        /**
         * @brief Sets the range used to compute 2D stereo panning.
         * @param units World-space range corresponding to full pan.
         */
        void SetPanRangeUnits(float units);
        /**
         * @brief Sets the number of pooled voices reserved for each sound effect.
         * @param voices Voice count per sound effect.
         */
        void SetSfxVoicesPerSound(ma_uint32 voices);

        /** @brief Unloads every managed sound asset. */
        void Clear();
        /** @brief Shuts the audio manager down and releases the miniaudio engine. */
        void shutdown();
    };
}

#endif // COSMIC_AUDIOMANAGER_HPP
