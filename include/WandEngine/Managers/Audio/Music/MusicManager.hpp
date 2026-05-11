#ifndef MUSICMANAGER_HPP
#define MUSICMANAGER_HPP

#include <FMOD/core/fmod.h>
#include <FMOD/core/fmod_common.h>
#include <unordered_map>
#include <string>

namespace WandEngine {

    class MusicManager {
    private:
        FMOD_SYSTEM* system;

        std::unordered_map<std::string, FMOD_SOUND*> musics;
        std::unordered_map<std::string, FMOD_CHANNEL*> channels;

        MusicManager();
        ~MusicManager();

    public:
        static MusicManager& GetInstance() {
            static MusicManager instance;
            return instance;
        }

        bool Init();
        void Update();
        bool Load(const std::string& key, const std::string& filename);
        bool Play(const std::string& key, float volume = 1.0f, bool loop = false);
        void Stop(const std::string& key);
        void StopAll();
        void SetVolume(const std::string& key, float volume);
        void SetPosition(const std::string& key, unsigned int milliseconds);
        void Clear();
        void Shutdown();

    };

}

#endif //MUSICMANAGER_HPP