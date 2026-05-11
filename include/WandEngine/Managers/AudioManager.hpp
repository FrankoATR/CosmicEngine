#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unordered_map>
#include <iostream>

namespace WandEngine
{
    class AudioEngine
    {
    private:
        ALLEGRO_VOICE *voice;
        ALLEGRO_MIXER *mixer;

        AudioEngine()
        {
            // Inicializa el subsistema de audio
            if (!al_install_audio())
            {
                std::cerr << "Error: No se pudo inicializar el sistema de audio." << std::endl;
            }
            if (!al_init_acodec_addon())
            {
                std::cerr << "Error: No se pudo inicializar los códecs de audio." << std::endl;
            }

            // Reserva muestras de audio
            if (!al_reserve_samples(16))
            {
                std::cerr << "Error: No se pudieron reservar muestras de audio." << std::endl;
            }

            // Crea el mezclador y la voz
            voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
            if (!voice)
            {
                std::cerr << "Error: No se pudo crear la voz de audio." << std::endl;
            }

            mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
            if (!mixer)
            {
                std::cerr << "Error: No se pudo crear el mezclador de audio." << std::endl;
            }

            if (!al_attach_mixer_to_voice(mixer, voice))
            {
                std::cerr << "Error: No se pudo conectar el mezclador a la voz." << std::endl;
            }
        }

        ~AudioEngine()
        {
            if (mixer)
                al_destroy_mixer(mixer);
            if (voice)
                al_destroy_voice(voice);
        }

    public:
        static AudioEngine &GetInstance()
        {
            static AudioEngine instance;
            return instance;
        }

        ALLEGRO_MIXER *GetMixer() const { return mixer; }
    };
}

#endif // AUDIOMANAGER_HPP
