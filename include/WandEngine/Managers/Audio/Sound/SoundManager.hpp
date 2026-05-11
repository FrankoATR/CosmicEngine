#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <FMOD/core/fmod.h>
#include <FMOD/core/fmod_common.h>
#include <unordered_map>
#include <string>

namespace WandEngine {

    class SoundManager {
    private:
        FMOD_SYSTEM* system;

        std::unordered_map<std::string, FMOD_SOUND*> sounds;
        std::unordered_map<std::string, FMOD_CHANNEL*> channels;

        SoundManager();
        ~SoundManager();

    public:
        static SoundManager& GetInstance() {
            static SoundManager instance;
            return instance;
        }

        bool Init();
        void Update();
        bool Load(const std::string& name, const std::string& filename);
        bool Play(const std::string& name, float volume = 1.0f, bool loop = false);
        void Stop(const std::string& name);
        void StopAll();
        void SetVolume(const std::string& name, float volume);
        void Clear();
        void Shutdown();

    };

}

#endif //SOUNDMANAGER_HPP