#include "SoundManager.hpp"

namespace WandEngine
{

    SoundManager::SoundManager() : mixer(nullptr)
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

        mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        if (!mixer)
        {
            std::cerr << "Failed to create mixer for SoundManager!" << std::endl;
        }

        if (!al_attach_mixer_to_mixer(mixer, al_get_default_mixer()))
        {
            std::cerr << "Failed to attach mixer to default mixer!" << std::endl;
        }
    }

    SoundManager::~SoundManager()
    {
        Clear();

        if (mixer)
        {
            al_destroy_mixer(mixer);
            mixer = nullptr;
        }

        if (al_is_audio_installed())
        {
            al_uninstall_audio();
        }

        std::cout << "Sound manager destroyed" << std::endl;
    }

    bool SoundManager::Load(const std::string &name, const std::string &filename)
    {
        if (sounds.find(name) != sounds.end())
        {
            std::cerr << "Sound already loaded: " << name << std::endl;
            return false;
        }

        ALLEGRO_SAMPLE *sample = al_load_sample(filename.c_str());
        if (!sample)
        {
            std::cerr << "Failed to load sound: " << filename << std::endl;
            return false;
        }

        sounds[name] = sample;
        return true;
    }

    bool SoundManager::Play(const std::string &name, float volume, bool loop)
    {
        auto it = sounds.find(name);
        if (it == sounds.end())
        {
            std::cerr << "Sound not found: " << name << std::endl;
            return false;
        }

        ALLEGRO_SAMPLE_INSTANCE *instance = al_create_sample_instance(it->second);
        if (!instance)
        {
            std::cerr << "Failed to create sample instance for sound: " << name << std::endl;
            return false;
        }

        al_set_sample_instance_gain(instance, volume);
        al_set_sample_instance_playmode(instance, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);

        if (!al_attach_sample_instance_to_mixer(instance, mixer))
        {
            std::cerr << "Failed to attach sample instance to mixer for sound: " << name << std::endl;
            al_destroy_sample_instance(instance);
            return false;
        }

        if (!al_play_sample_instance(instance))
        {
            std::cerr << "Failed to play sound: " << name << std::endl;
            al_destroy_sample_instance(instance);
            return false;
        }

        instances[name] = instance;
        return true;
    }

    void SoundManager::Stop(const std::string &name)
    {
        auto it = instances.find(name);
        if (it != instances.end())
        {
            al_stop_sample_instance(it->second);
            al_destroy_sample_instance(it->second);
            instances.erase(it);
        }
    }

    void SoundManager::StopAll()
    {
        for (auto &pair : instances)
        {
            al_stop_sample_instance(pair.second);
            al_destroy_sample_instance(pair.second);
        }
        instances.clear();
    }

    void SoundManager::SetVolume(const std::string &name, float volume)
    {
        auto it = instances.find(name);
        if (it != instances.end())
        {
            al_set_sample_instance_gain(it->second, volume);
        }
    }

    void SoundManager::Clear()
    {
        StopAll();

        for (auto &pair : sounds)
        {
            al_destroy_sample(pair.second);
        }
        sounds.clear();

        std::cout << "Sound manager cleared" << std::endl;
    }

}
