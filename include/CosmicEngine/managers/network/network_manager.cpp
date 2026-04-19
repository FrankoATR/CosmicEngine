/**
 * @file network_manager.cpp
 * @brief Implements the ENet-based client/server networking manager.
 */

#include "network_manager.hpp"
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
        RUNTIME_LIFECYCLE("Network manager", "created");
    }

    NetworkManager::~NetworkManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Network manager", "destroyed");
    }

    bool NetworkManager::init()
    {
        if (enetInitialized)
        {
            return true;
        }

        if (enet_initialize() != 0)
        {
            RUNTIME_WARNING("[NetworkManager] Failed to initialize ENet.");
            return false;
        }
        enetInitialized = true;
        RUNTIME_LIFECYCLE("Network manager", "initialized");
        return true;
    }

    void NetworkManager::shutdown()
    {
        Disconnect();
        if (enetInitialized)
        {
            enet_deinitialize();
            enetInitialized = false;
        }
        RUNTIME_LIFECYCLE("Network manager", "shutdown");
    }

    // ──────────────────────────────────────────────
    // Server
    // ──────────────────────────────────────────────

    bool NetworkManager::StartServer(int port)
    {
        if (!enetInitialized && !init())
        {
            return false;
        }

        if (role != NetworkRole::None)
        {
            RUNTIME_WARNING("[NetworkManager] Already running as " << (role == NetworkRole::Server ? "server" : "client") << ".");
            return false;
        }

        ENetAddress address = {};
        address.host = ENET_HOST_ANY;
        address.port = static_cast<enet_uint16>(port);

        host = enet_host_create(&address, kMaxClients, 2, 0, 0);
        if (!host)
        {
            RUNTIME_WARNING("[NetworkManager] Failed to create ENet server host on port " << port << ".");
            return false;
        }

        role = NetworkRole::Server;
        localClientId = 0; // Server is always peer 0
        running.store(true);

        networkThread = std::thread(&NetworkManager::ServerLoop, this);

        RUNTIME_INFO("[NetworkManager] Server started on port " << port << ".");
        return true;
    }

    void NetworkManager::ServerLoop()
    {
        ENetEvent event = {};

		// Network I/O stays on a dedicated thread and forwards decoded messages to the game thread through thread-safe queues.
        while (running.load())
        {
            while (enet_host_service(host, &event, 10) > 0)
            {
                if (!running.load())
                    break;

                switch (event.type)
                {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    int peerId = session.AddPeer();
                    peerIdMap[event.peer] = peerId;
                    event.peer->data = reinterpret_cast<void *>(static_cast<intptr_t>(peerId));

                    // Send the client their assigned ID
                    NetworkMessage assignMsg(NetMessageType::AssignClientId, {{"clientId", peerId}});
                    SendPacketTo(event.peer, assignMsg);

                    // Notify all existing peers about the new connection
                    NetworkMessage connectMsg(NetMessageType::ClientConnect, {{"clientId", peerId}});
                    BroadcastToAll(connectMsg);

                    // Send existing peer list to the new client
                    for (const auto &existingPeer : session.GetAllPeers())
                    {
                        if (existingPeer.id != peerId)
                        {
                            NetworkMessage peerInfo(NetMessageType::ClientConnect, {
                                {"clientId", existingPeer.id},
                                {"username", existingPeer.username}
                            });
                            SendPacketTo(event.peer, peerInfo);
                        }
                    }

                    // Push to game thread
                    incomingQueue.Push({peerId, connectMsg});

                    RUNTIME_INFO("[NetworkManager] Client " << peerId << " connected.");
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    int peerId = static_cast<int>(reinterpret_cast<intptr_t>(event.peer->data));
                    NetworkMessage msg = NetworkMessage::Deserialize(event.packet->data, event.packet->dataLength);

                    HandleServerMessage(event.peer, peerId, msg);

                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    int peerId = static_cast<int>(reinterpret_cast<intptr_t>(event.peer->data));

                    NetworkMessage disconnectMsg(NetMessageType::ClientDisconnect, {{"clientId", peerId}});
                    BroadcastToAll(disconnectMsg, event.peer);
                    incomingQueue.Push({peerId, disconnectMsg});

                    session.RemovePeer(peerId);
                    peerIdMap.erase(event.peer);
                    event.peer->data = nullptr;

                    RUNTIME_INFO("[NetworkManager] Client " << peerId << " disconnected.");
                    break;
                }

                default:
                    break;
                }
            }
        }
    }

    void NetworkManager::HandleServerMessage(ENetPeer *peer, int peerId, const NetworkMessage &msg)
    {
        switch (msg.type)
        {
        case NetMessageType::ChatMessage:
        case NetMessageType::PlayerState:
        case NetMessageType::SpawnObject:
        case NetMessageType::DestroyObject:
        case NetMessageType::ObjectState:
        case NetMessageType::DamageEvent:
        case NetMessageType::PlayerDeath:
        {
            // Stamp the sender ID and relay to all clients (including sender for confirmation)
            nlohmann::json relayPayload = msg.payload;
            relayPayload["senderId"] = peerId;
            NetworkMessage relayMsg(msg.type, relayPayload);
            BroadcastToAll(relayMsg);

            // Also push to game thread for server-side logic
            incomingQueue.Push({peerId, relayMsg});
            break;
        }

        case NetMessageType::Ping:
        {
            NetworkMessage pong(NetMessageType::Pong, msg.payload);
            SendPacketTo(peer, pong);
            break;
        }

        default:
        {
            // Forward custom/unknown messages to game thread and relay
            nlohmann::json relayPayload = msg.payload;
            relayPayload["senderId"] = peerId;
            NetworkMessage relayMsg(msg.type, relayPayload);
            BroadcastToAll(relayMsg);
            incomingQueue.Push({peerId, relayMsg});
            break;
        }
        }
    }

    // ──────────────────────────────────────────────
    // Client
    // ──────────────────────────────────────────────

    bool NetworkManager::ConnectToServer(const std::string &ip, int port, const std::string &username)
    {
        if (!enetInitialized && !init())
        {
            return false;
        }

        if (role != NetworkRole::None)
        {
            RUNTIME_WARNING("[NetworkManager] Already running as " << (role == NetworkRole::Server ? "server" : "client") << ".");
            return false;
        }

        host = enet_host_create(nullptr, 1, 2, 0, 0);
        if (!host)
        {
            RUNTIME_WARNING("[NetworkManager] Failed to create ENet client host.");
            return false;
        }

        ENetAddress address = {};
        enet_address_set_host(&address, ip.c_str());
        address.port = static_cast<enet_uint16>(port);

        serverPeer = enet_host_connect(host, &address, 2, 0);
        if (!serverPeer)
        {
            RUNTIME_WARNING("[NetworkManager] No available peers for connection.");
            enet_host_destroy(host);
            host = nullptr;
            return false;
        }

        // Wait for connection
        ENetEvent event = {};
        if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            RUNTIME_INFO("[NetworkManager] Connected to " << ip << ":" << port << ".");
        }
        else
        {
            RUNTIME_WARNING("[NetworkManager] Connection to " << ip << ":" << port << " failed.");
            enet_peer_reset(serverPeer);
            enet_host_destroy(host);
            host = nullptr;
            serverPeer = nullptr;
            return false;
        }

        localUsername = username;
        role = NetworkRole::Client;
        running.store(true);

        networkThread = std::thread(&NetworkManager::ClientLoop, this);

        // Send username to server
        NetworkMessage greetMsg(NetMessageType::ChatMessage, {{"username", username}, {"type", "join"}});
        SendToServer(greetMsg);

        return true;
    }

    void NetworkManager::ClientLoop()
    {
        ENetEvent event = {};

        while (running.load())
        {
            while (enet_host_service(host, &event, 10) > 0)
            {
                if (!running.load())
                    break;

                switch (event.type)
                {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    NetworkMessage msg = NetworkMessage::Deserialize(event.packet->data, event.packet->dataLength);
                    HandleClientMessage(msg);
                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    RUNTIME_INFO("[NetworkManager] Disconnected from server.");
                    incomingQueue.Push({-1, NetworkMessage(NetMessageType::ClientDisconnect, {{"clientId", localClientId}})});
                    running.store(false);
                    return;
                }

                default:
                    break;
                }
            }
        }
    }

    void NetworkManager::HandleClientMessage(const NetworkMessage &msg)
    {
        switch (msg.type)
        {
        case NetMessageType::AssignClientId:
        {
            localClientId = msg.payload.value("clientId", -1);
            RUNTIME_INFO("[NetworkManager] Assigned client ID: " << localClientId);
            incomingQueue.Push({-1, msg});
            break;
        }

        case NetMessageType::ClientConnect:
        {
            int peerId = msg.payload.value("clientId", -1);
            std::string username = msg.payload.value("username", "");
            if (peerId >= 0)
            {
                session.AddPeer(username);
            }
            incomingQueue.Push({peerId, msg});
            break;
        }

        case NetMessageType::ClientDisconnect:
        {
            int peerId = msg.payload.value("clientId", -1);
            session.RemovePeer(peerId);
            incomingQueue.Push({peerId, msg});
            break;
        }

        default:
        {
            int senderId = msg.payload.value("senderId", -1);
            incomingQueue.Push({senderId, msg});
            break;
        }
        }
    }

    // ──────────────────────────────────────────────
    // Disconnect
    // ──────────────────────────────────────────────

    void NetworkManager::Disconnect()
    {
        if (role == NetworkRole::None)
            return;

        running.store(false);

        if (networkThread.joinable())
            networkThread.join();

        if (role == NetworkRole::Client && serverPeer)
        {
            enet_peer_disconnect(serverPeer, 0);
            
            // Allow some time for the disconnect to be sent
            ENetEvent event = {};
            while (enet_host_service(host, &event, 500) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_RECEIVE)
                    enet_packet_destroy(event.packet);
                else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                    break;
            }
            serverPeer = nullptr;
        }

        if (host)
        {
            enet_host_destroy(host);
            host = nullptr;
        }

        session.Clear();
        peerIdMap.clear();
        incomingQueue.Clear();
        outgoingQueue.Clear();

        role = NetworkRole::None;
        localClientId = -1;
        localUsername.clear();

        RUNTIME_INFO("[NetworkManager] Disconnected.");
    }

    // ──────────────────────────────────────────────
    // Message processing (game thread)
    // ──────────────────────────────────────────────

    void NetworkManager::ProcessMessages()
    {
		// Handlers always run on the game thread so scene code can safely mutate engine state.
        IncomingMessage incoming;
        while (incomingQueue.Pop(incoming))
        {
            auto it = handlers.find(incoming.message.type);
            if (it != handlers.end())
            {
                for (const auto &handler : it->second)
                {
                    handler(incoming.senderId, incoming.message.payload);
                }
            }
        }
    }

    void NetworkManager::RegisterHandler(NetMessageType type, MessageHandler handler)
    {
        handlers[type].push_back(std::move(handler));
    }

    void NetworkManager::ClearHandlers(NetMessageType type)
    {
        handlers.erase(type);
    }

    void NetworkManager::ClearAllHandlers()
    {
        handlers.clear();
    }

    // ──────────────────────────────────────────────
    // Sending
    // ──────────────────────────────────────────────

    void NetworkManager::SendPacketTo(ENetPeer *peer, const NetworkMessage &msg, bool reliable)
    {
        auto data = msg.Serialize();
        enet_uint32 flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
        ENetPacket *packet = enet_packet_create(data.data(), data.size(), flags);
        enet_peer_send(peer, 0, packet);
    }

    void NetworkManager::BroadcastToAll(const NetworkMessage &msg, ENetPeer *exclude, bool reliable)
    {
        auto data = msg.Serialize();
        enet_uint32 flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0;

        for (size_t i = 0; i < host->peerCount; ++i)
        {
            ENetPeer *peer = &host->peers[i];
            if (peer->state == ENET_PEER_STATE_CONNECTED && peer != exclude)
            {
                ENetPacket *packet = enet_packet_create(data.data(), data.size(), flags);
                enet_peer_send(peer, 0, packet);
            }
        }
    }

    void NetworkManager::SendToServer(const NetworkMessage &msg)
    {
        if (role != NetworkRole::Client || !serverPeer)
        {
            RUNTIME_WARNING("[NetworkManager] Not connected as client.");
            return;
        }
        SendPacketTo(serverPeer, msg);
    }

    void NetworkManager::SendToClient(int peerId, const NetworkMessage &msg)
    {
        if (role != NetworkRole::Server)
        {
            RUNTIME_WARNING("[NetworkManager] Not running as server.");
            return;
        }

        for (const auto &[peer, id] : peerIdMap)
        {
            if (id == peerId)
            {
                SendPacketTo(peer, msg);
                return;
            }
        }
        RUNTIME_WARNING("[NetworkManager] Peer " << peerId << " not found.");
    }

    void NetworkManager::Broadcast(const NetworkMessage &msg, int excludePeerId)
    {
        if (role != NetworkRole::Server)
        {
            RUNTIME_WARNING("[NetworkManager] Not running as server.");
            return;
        }

        ENetPeer *excludePeer = nullptr;
        if (excludePeerId >= 0)
        {
            for (const auto &[peer, id] : peerIdMap)
            {
                if (id == excludePeerId)
                {
                    excludePeer = peer;
                    break;
                }
            }
        }

        BroadcastToAll(msg, excludePeer);
    }

    // ──────────────────────────────────────────────
    // Queries
    // ──────────────────────────────────────────────

    NetworkRole NetworkManager::GetRole() const
    {
        return role;
    }

    bool NetworkManager::IsConnected() const
    {
        return role != NetworkRole::None;
    }

    bool NetworkManager::IsServer() const
    {
        return role == NetworkRole::Server;
    }

    bool NetworkManager::IsClient() const
    {
        return role == NetworkRole::Client;
    }

    int NetworkManager::GetLocalClientId() const
    {
        return localClientId;
    }

    const NetworkSession &NetworkManager::GetSession() const
    {
        return session;
    }

    size_t NetworkManager::GetPeerCount() const
    {
        return session.GetPeerCount();
    }

}
