#ifndef COSMIC_NETWORKMANAGER_HPP
#define COSMIC_NETWORKMANAGER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <charconv>
#include <cstring>
#include <map>
#include <thread>
#include <atomic>

#define NOMINMAX
#define byte WindowsByte
#include <enet/enet.h>
#undef byte

using namespace std;

#define MAX_CLIENTS 32


namespace CosmicEngine
{

    class NetworkManager
    {
    private:
        class ClientData
        {
        private:
            int m_id;
            string m_username;

        public:
            ClientData(int id) : m_id{id} {}

            void SetUsername(string username) { m_username = username; }

            int GetID() { return m_id; }

            string GetUsername() { return m_username; }
        };

        map<int, ClientData *> client_map;

        int server_port = 25565;
        ENetAddress address = {};
        ENetEvent event = {};


        int new_player_id;

        thread t;
        char server_ip[20] = "127.0.0.1";

        std::atomic<bool> g_running{true};

        NetworkManager();
        ~NetworkManager();
        NetworkManager(const NetworkManager &) = delete;
        NetworkManager &operator=(const NetworkManager &) = delete;


        void doAccept();
        void doRead();

    public:
        static NetworkManager &GetInstance();

        bool Init_server();
        void update_server();

        bool Init_client();
        void update_client();

        void _message_loop_(ENetHost *client);

        void shutdown();

        ENetHost *server = nullptr;
        ENetHost *client = nullptr;
        ENetPeer *peer = nullptr;
        int CLIENT_ID;

        bool ReadConsoleLine(const char *prompt, char *buffer, size_t max);
        void BroadcastPacket(ENetHost *server, const char *data);
        void SendPacket(ENetPeer *peer, const char *data);
        void ClientParseData(const char *data);
        void ServerParseData(ENetHost *server, int id, const char *data);


        void StartServer();
        void broadcast();
    };

}

#endif // COSMIC_NETWORKMANAGER_HPP