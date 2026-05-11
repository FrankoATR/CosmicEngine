#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unordered_map>
#include <string>
#include <iostream>

namespace WandEngine
{

    class SoundManager
    {
    private:
        SoundManager();
        ~SoundManager();

        SoundManager(const SoundManager &) = delete;
        SoundManager &operator=(const SoundManager &) = delete;

        std::unordered_map<std::string, ALLEGRO_SAMPLE *> sounds;
        std::unordered_map<std::string, ALLEGRO_SAMPLE_INSTANCE *> instances;

        ALLEGRO_MIXER *mixer;

    public:
        static SoundManager &GetInstance()
        {
            static SoundManager instance;
            return instance;
        }

        bool Load(const std::string &name, const std::string &filename);
        bool Play(const std::string &name, float volume = 1.0f, bool loop = false);
        void Stop(const std::string &name);
        void StopAll();
        void SetVolume(const std::string &name, float volume);
        void Clear();
    };

}

#endif // SOUNDMANAGER_HPP
