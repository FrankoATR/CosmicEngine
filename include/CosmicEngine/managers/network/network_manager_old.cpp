#include "network_manager.hpp"
#include "../object/object_manager.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{

    NetworkManager &NetworkManager::GetInstance()
    {
        static NetworkManager instance;
        return instance;
    }

    NetworkManager::NetworkManager()
    {
		RUNTIME_LIFECYCLE("Legacy network manager", "created");
    }

    NetworkManager::~NetworkManager()
    {
		RUNTIME_LIFECYCLE("Legacy network manager", "destroyed");
    }

    bool NetworkManager::Init_server()
    {
        if (enet_initialize() != 0)
        {
			RUNTIME_WARNING("[LegacyNetworkManager] An error occurred while initializing ENet.");
            return EXIT_FAILURE;
        }

        atexit(enet_deinitialize);

        address.host = ENET_HOST_ANY;
        address.port = server_port;

        server = enet_host_create(
            &address,
            MAX_CLIENTS,
            1,
            0,
            0);

        if (!server)
        {
			RUNTIME_WARNING("[LegacyNetworkManager] Failed to create an ENet server host.");
            return EXIT_FAILURE;
        }

        new_player_id = 0;

        t = std::thread([this]()
                        {
            while (g_running.load()) {
                this->update_server();
            } });

        RUNTIME_LIFECYCLE("Legacy network manager", "initialized");
        return true;
    }

    bool NetworkManager::Init_client()
    {

        if (enet_initialize() != 0)
        {
			RUNTIME_WARNING("[LegacyNetworkManager] An error occurred while initializing ENet.");
            return EXIT_FAILURE;
        }

        atexit(enet_deinitialize);

        client = enet_host_create(
            NULL,
            1,
            1,
            0,
            0);

        if (!client)
        {
			RUNTIME_WARNING("[LegacyNetworkManager] Failed to create an ENet client host.");
            return EXIT_FAILURE;
        }

        enet_address_set_host(&address, server_ip);
        address.port = server_port;

        peer = enet_host_connect(client, &address, 1, 0);

        if (!peer)
        {
			RUNTIME_WARNING("[LegacyNetworkManager] No available peers for initiating an ENet connection.");
            return EXIT_FAILURE;
        }

        if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
			RUNTIME_INFO("[LegacyNetworkManager] Connection to " << server_ip << ":" << server_port << " succeeded.");
        }
        else
        {
            enet_peer_reset(peer);
			RUNTIME_WARNING("[LegacyNetworkManager] Connection to " << server_ip << ":" << server_port << " failed.");
            return EXIT_SUCCESS;
        }

        CLIENT_ID = -1;

        t = thread(&NetworkManager::_message_loop_, this, client);

        char init_data[80];
        snprintf(init_data, sizeof(init_data),
                 "2|%s",
                 "FRANKO");
        SendPacket(peer, init_data);
        return true;
    }

    bool NetworkManager::ReadConsoleLine(const char *prompt, char *buffer, size_t max)
    {
        if (prompt)
            printf("%s", prompt);

        if (!fgets(buffer, max, stdin))
        {
            printf("Failed to read input.\n");
            return false;
        }

        size_t len = strcspn(buffer, "\n");
        if (len < max && buffer[len] == '\n')
            buffer[len] = '\0';
        else
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
        }

        if (strlen(buffer) == 0)
        {
            printf("This value cannot be empty.\n");
            return false;
        }
        if (strlen(buffer) >= max - 1)
        {
            printf("Text is too long (max %zu characters).\n", max - 1);
            return false;
        }
        return true;
    }

    void NetworkManager::BroadcastPacket(ENetHost *server, const char *data)
    {
        ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(server, 0, packet);
    }

    void NetworkManager::SendPacket(ENetPeer *peer, const char *data)
    {
        ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }

    void NetworkManager::ClientParseData(const char *data)
    {
        // printf("Parse: %s\n", data);

        int data_type;
        int id;
        sscanf(data, "%d|%d", &data_type, &id);

        switch (data_type)
        {
        case 1:
            if (!client_map.count(id))
                break;

            if (id != CLIENT_ID)
            {
                char msg[80];
                sscanf(data, "%*d|%*d|%[^|]", &msg);
                printf("%s: %s\n", client_map[id]->GetUsername().c_str(), msg);
            }
            break;

        case 2:
            if (id != CLIENT_ID)
            {
                char username[80];
                sscanf(data, "%*d|%*d|%[^|]", username);
                if (!client_map.count(id))
                    client_map[id] = new ClientData(id);
                client_map[id]->SetUsername(username);
            }
            break;

        case 3:
            CLIENT_ID = id;
            break;

        case 4:
        {
            if (!client_map.count(id))
                break;
            ClientData *tmp = client_map[id];
            client_map.erase(id);
            delete tmp;
        }
        case 5:
        {
            int id, x, y, z;
            sscanf(data, "5|%d|%d|%d|%d", &id, &x, &y, &z);
            if (id != CLIENT_ID)
            {
                //ObjectManager::GetInstance().Add(new Cube(glm::vec3(x, y, z), glm::vec3(1.0f)));
            }
            break;
        }

        case 6:
        {
            int id, x, y, z;
            sscanf(data, "6|%d|%d|%d|%d", &id, &x, &y, &z);
            if (id != CLIENT_ID)
            {
                /*
                for (auto obj : ObjectManager::GetInstance().FindByPosition(glm::vec3(x, y, z)))
                {
                    obj->Destroy();
                }
                */
            }
            break;
        }

        default:
            break;
        }
    }

    void NetworkManager::ServerParseData(ENetHost *server, int id, const char *data)
    {
        // printf("Parse: %s\n", data);

        const char *pipe_data_type = strchr(data, '|');
        if (!pipe_data_type)
            return;

        int data_type = 0;
        for (const char *p = data; p < pipe_data_type; ++p)
        {
            if (*p < '0' || *p > '9')
                break;
            data_type = data_type * 10 + (*p - '0');
        }

        // printf("value: %d\n", data_type);

        switch (data_type)
        {
        case 1:
        {

            auto it = client_map.find(id);
            if (it == client_map.end())
                break;

            char msg[80];
            sscanf(data, "%*d|%[^\n]", msg);

            printf("%s: %s\n",
                   it->second->GetUsername().c_str(),
                   msg);

            char send_data[1024];
            snprintf(send_data, sizeof(send_data),
                     "1|%d|%s",
                     id, msg);

            BroadcastPacket(server, send_data);

            break;
        }

        case 2:
        {
            char username[80];

            sscanf(data, "2|%[^\n]", username);
            // sscanf(data, "2|%79[^\n]", username);

            char send_data[1024] = {'\0'};
            sprintf(send_data, "2|%d|%s", id, username);
            printf("Send: %s\n", send_data);

            BroadcastPacket(server, send_data);
            client_map[id]->SetUsername(username);

            break;
        }
        case 5: // place block
        {
            int id, x, y, z;
            sscanf(data, "5|%d|%d|%d|%d", &id, &x, &y, &z);
            if (id != CLIENT_ID)
            {
                //ObjectManager::GetInstance().Add(new Cube(glm::vec3(x, y, z), glm::vec3(1.0f)));
            }
            printf("Player %d placed a block at (%d,%d,%d)\n", id, x, y, z);

            // forward to every other peer
            char send_data[64];
            snprintf(send_data, sizeof(send_data), "5|%d|%d|%d|%d", id, x, y, z);
            BroadcastPacket(server, send_data);
            break;
        }
        case 6:
        {
            int id, x, y, z;
            sscanf(data, "6|%d|%d|%d|%d", &id, &x, &y, &z);
            if (id != CLIENT_ID)
            {
                /*
                for (auto obj : ObjectManager::GetInstance().FindByPosition(glm::vec3(x, y, z)))
                {
                    obj->Destroy();
                }
                */
            }
            printf("Player %d destroyed a block at (%d,%d,%d)\n", id, x, y, z);

            // forward to every other peer
            char send_data[64];
            snprintf(send_data, sizeof(send_data), "6|%d|%d|%d|%d", id, x, y, z);
            BroadcastPacket(server, send_data);
        }

        default:
            return;
            break;
        }
    }

    void NetworkManager::update_server()
    {
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {

                printf("A new client connected from %x:%u\n",
                       event.peer->address.host,
                       event.peer->address.port);

                for (auto const &x : client_map)
                {
                    char send_data[1024] = {'\0'};
                    sprintf(send_data, "2|%d|%s", x.first, x.second->GetUsername().c_str());
                    BroadcastPacket(server, send_data);
                }

                new_player_id++;

                client_map[new_player_id] = new ClientData(new_player_id);
                event.peer->data = client_map[new_player_id];

                char data_to_send[126] = {'\0'};

                sprintf(data_to_send, "3|%d", new_player_id);

                SendPacket(event.peer, data_to_send);

                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                /*
                                printf("Packet [%s] (%u bytes) from %x:%u on channel %u\n",
                                       event.packet->data,
                                       event.packet->dataLength,
                                       event.peer->address.host,
                                       event.peer->address.port,
                                       event.channelID);
                */

                ServerParseData(server, static_cast<ClientData *>(event.peer->data)->GetID(), reinterpret_cast<const char *>(event.packet->data));

                enet_packet_destroy(event.packet);

                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("%x:%u disconnected\n",
                       event.peer->address.host,
                       event.peer->address.port);

                char disconnect_data[126] = {'\0'};
                sprintf(disconnect_data, "4|%d", static_cast<ClientData *>(event.peer->data)->GetID());
                BroadcastPacket(server, disconnect_data);

                event.peer->data = nullptr;

                break;
            }

            default:
                break;
            }
        }
    }

    void NetworkManager::_message_loop_(ENetHost *client)
    {
        ENetEvent event = {};

        while (g_running.load())
        {
            while (enet_host_service(client, &event, 0) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    /*
                        string receivedMsg(reinterpret_cast<const char *>(event.packet->data), event.packet->dataLength);

                        char ipStr[128];
                        enet_address_get_host_ip(&event.peer->address, ipStr, sizeof(ipStr));
                        string receivedHost(ipStr);

                        cout << "A packet of length " << event.packet->dataLength
                                << " containing \"" << receivedMsg << "\" was received for "
                                << receivedHost << ":" << event.peer->address.port
                                << " on channel " << event.channelID << "\n";
                    */

                    ClientParseData(reinterpret_cast<const char *>(event.packet->data));

                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("Desconectado del servidor.\n");
                    g_running.store(false);
                    return;

                default:
                    break;
                }
            }
        }
    }

    void NetworkManager::shutdown()
    {
        g_running.store(false);
        if (t.joinable())
            t.join();
    }

    void NetworkManager::StartServer()
    {
    }

    void NetworkManager::doAccept()
    {
    }

    void NetworkManager::broadcast()
    {
    }

}
