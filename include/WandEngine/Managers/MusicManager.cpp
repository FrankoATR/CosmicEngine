#include "MusicManager.hpp"

namespace WandEngine
{

    MusicManager::MusicManager() : currentMusic(nullptr), voice(nullptr), mixer(nullptr)
    {
        if (!al_is_audio_installed())
        {
            if (!al_install_audio())
            {
                std::cerr << "Failed to initialize audio!" << std::endl;
            }
        }

        if (!al_is_acodec_addon_initialized())
        {
            if (!al_init_acodec_addon())
            {
                std::cerr << "Failed to initialize audio codecs!" << std::endl;
            }
        }

        al_reserve_samples(10);

        voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
        if (!voice)
        {
            std::cerr << "Failed to create voice" << std::endl;
        }

        mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        if (!mixer)
        {
            std::cerr << "Failed to create mixer" << std::endl;
        }

        if (!al_attach_mixer_to_voice(mixer, voice))
        {
            std::cerr << "Failed to attach mixer to voice" << std::endl;
        }
    }

    MusicManager::~MusicManager()
    {
        Clear();

        if (mixer)
        {
            //al_destroy_mixer(mixer);
            mixer = nullptr;
        }

        if (voice)
        {
            //al_destroy_voice(voice);
            voice = nullptr;
        }

        if(al_is_audio_installed())
        {
            al_uninstall_audio();
        }

        std::cout << "Music manager destroyed" << std::endl;
    }

    bool MusicManager::Load(const std::string &name, const std::string &filename)
    {
        if (musicTracks.find(name) != musicTracks.end())
        {
            std::cerr << "Music track already loaded: " << name << std::endl;
            return false;
        }

        ALLEGRO_AUDIO_STREAM *stream = al_load_audio_stream(filename.c_str(), 4, 2048);
        if (!stream)
        {
            std::cerr << "Failed to load music: " << filename << std::endl;
            return false;
        }

        if (!al_attach_audio_stream_to_mixer(stream, mixer))
        {
            std::cerr << "Failed to attach audio stream to mixer!" << std::endl;
            al_destroy_audio_stream(stream);
            return false;
        }

        al_set_audio_stream_playing(stream, false);

        musicTracks[name] = stream;

        return true;
    }

    bool MusicManager::Play(const std::string &name, float volume, bool loop)
    {
        auto it = musicTracks.find(name);
        if (it == musicTracks.end())
        {
            std::cerr << "Music track not found: " << name << std::endl;
            return false;
        }

        if (currentMusic && currentMusic != it->second)
        {
            al_set_audio_stream_playing(currentMusic, false);
        }

        currentMusic = it->second;

        al_set_audio_stream_gain(currentMusic, volume);

        ALLEGRO_PLAYMODE playmode = loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE;
        al_set_audio_stream_playmode(currentMusic, playmode);

        al_set_audio_stream_playing(currentMusic, true);

        return true;
    }

    void MusicManager::Pause()
    {
        if (currentMusic)
        {
            al_set_audio_stream_playing(currentMusic, false);
        }
    }

    void MusicManager::Resume()
    {
        if (currentMusic)
        {
            al_set_audio_stream_playing(currentMusic, true);
        }
    }

    void MusicManager::Stop()
    {
        if (currentMusic)
        {
            al_set_audio_stream_playing(currentMusic, false);
            currentMusic = nullptr;
        }
    }

    void MusicManager::Restart()
    {
        if (currentMusic)
        {
            if (!al_seek_audio_stream_secs(currentMusic, 0))
            {
                std::cerr << "Failed to restart the current music track" << std::endl;
                return;
            }

            al_set_audio_stream_playing(currentMusic, true);
        }
        else
        {
            std::cerr << "No music track is currently playing to restart" << std::endl;
        }
    }


    void MusicManager::Clear()
    {
        for (auto &pair : musicTracks)
        {
            al_set_audio_stream_playing(pair.second, false);
            al_detach_audio_stream(pair.second);
            al_destroy_audio_stream(pair.second);
        }
        musicTracks.clear();
        currentMusic = nullptr;

        std::cout << "Music manager cleared" << std::endl;
    }

}
