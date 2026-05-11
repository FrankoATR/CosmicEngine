#ifndef MUSICMANAGER_HPP
#define MUSICMANAGER_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unordered_map>
#include <string>
#include <iostream>

namespace WandEngine
{

    class MusicManager
    {
    private:
        MusicManager();
        ~MusicManager();

        MusicManager(const MusicManager &) = delete;
        MusicManager &operator=(const MusicManager &) = delete;

        std::unordered_map<std::string, ALLEGRO_AUDIO_STREAM *> musicTracks;

        ALLEGRO_AUDIO_STREAM *currentMusic;

        ALLEGRO_VOICE *voice;
        ALLEGRO_MIXER *mixer;

    public:
        static MusicManager &GetInstance()
        {
            static MusicManager instance;
            return instance;
        }

        bool LoadMusic(const std::string &name, const std::string &filename);

        bool PlayMusic(const std::string &name, float volume = 1.0f, bool loop = true);

        void PauseMusic();

        void ResumeMusic();

        void StopMusic();

        void Clear();
    };

}

#endif // MUSICMANAGER_HPP
