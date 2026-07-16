#ifndef PUZZLERPG_GAME_SESSION_HPP
#define PUZZLERPG_GAME_SESSION_HPP

#include <string>

namespace PuzzleRPG
{
    // Estado global ligero compartido entre escenas (menu -> lobby -> juego).
    // Vive como singleton accedido solo desde el hilo principal del juego.
    struct GameSession
    {
        enum class NetworkMode { SinglePlayer, Host, Client };

        NetworkMode mode = NetworkMode::SinglePlayer;
        std::string hostIp     = "127.0.0.1";
        int         port       = 25566;
        std::string username   = "Player";
        std::string selectedLevel; // nombre sin extension

        static GameSession &Get()
        {
            static GameSession instance;
            return instance;
        }
    };
}

#endif
