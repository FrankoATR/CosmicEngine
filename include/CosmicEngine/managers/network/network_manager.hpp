#ifndef COSMIC_NETWORKMANAGER_HPP
#define COSMIC_NETWORKMANAGER_HPP

/**
 * @file network_manager.hpp
 * @brief Declares the ENet-based network manager and supporting types.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define byte WindowsByte
#include <enet/enet.h>
#undef byte

#include "network_message.hpp"
#include "network_session.hpp"

namespace CosmicEngine
{

    /**
     * @brief The role of the local network host.
     */
    enum class NetworkRole
    {
        None,
        Server,
        Client
    };

    /**
     * @brief Thread-safe queue for passing data between the network thread and the game thread.
     * @tparam T Stored value type.
     */
    template <typename T>
    class ThreadSafeQueue
    {
    private:
        std::queue<T> queue;
        mutable std::mutex mutex;

    public:
        /**
         * @brief Pushes a new item into the queue.
         * @param item Item to enqueue.
         */
        void Push(T item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(item));
        }

        /**
         * @brief Pops the next item from the queue.
         * @param outItem Output item when one is available.
         * @return True when an item was removed from the queue.
         */
        bool Pop(T &outItem)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty())
                return false;
            outItem = std::move(queue.front());
            queue.pop();
            return true;
        }

        /**
         * @brief Returns whether the queue is empty.
         * @return True when no items are stored.
         */
        bool Empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        /**
         * @brief Returns the number of items currently stored.
         * @return Queue size.
         */
        size_t Size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

        /** @brief Removes every queued item. */
        void Clear()
        {
            std::lock_guard<std::mutex> lock(mutex);
            while (!queue.empty())
                queue.pop();
        }
    };

    /**
     * @brief An incoming message with the sender's peer ID.
     */
    struct IncomingMessage
    {
        /** @brief Session identifier of the message sender. */
        int senderId;
        /** @brief Message payload decoded from ENet transport. */
        NetworkMessage message;
    };

    /**
     * @brief Callback type for handling specific message types.
     */
    using MessageHandler = std::function<void(int senderId, const nlohmann::json &payload)>;

    /**
     * @brief Maximum number of clients that can connect to a server.
     */
    constexpr int kMaxClients = 32;

    /**
     * @brief Default server port.
     */
    constexpr int kDefaultPort = 25565;

    /**
     * @class NetworkManager
     * @brief Manages ENet-based networking with a clean client-server architecture.
     * 
     * Features:
     * - JSON-based message protocol via NetworkMessage
     * - Thread-safe message queue between network and game threads
     * - Session management for connected peers
     * - Callback-based message handling with RegisterHandler()
     * - Support for both server and client roles
     * 
     * Usage:
     * @code
     * // Start as server
     * NetworkManager::GetInstance().StartServer(25565);
     * 
     * // Or start as client
     * NetworkManager::GetInstance().ConnectToServer("127.0.0.1", 25565, "PlayerName");
     * 
     * // Register message handlers
     * NetworkManager::GetInstance().RegisterHandler(NetMessageType::ChatMessage, 
     *     [](int senderId, const nlohmann::json& payload) {
     *         std::cout << payload["message"].get<std::string>() << std::endl;
     *     });
     * 
     * // Send messages
     * NetworkManager::GetInstance().SendToServer(NetworkMessage(NetMessageType::ChatMessage, {{"message", "Hello!"}}));
     * @endcode
     */
    class NetworkManager
    {
    private:
        NetworkManager();
        ~NetworkManager();
        NetworkManager(const NetworkManager &) = delete;
        NetworkManager &operator=(const NetworkManager &) = delete;

        // ENet state
        ENetHost *host = nullptr;
        ENetPeer *serverPeer = nullptr;
        
        // Session
        NetworkSession session;
        NetworkRole role = NetworkRole::None;
        int localClientId = -1;
        std::string localUsername;

        // Threading
        std::thread networkThread;
        std::atomic<bool> running{false};

        // Thread-safe message queues
        ThreadSafeQueue<IncomingMessage> incomingQueue;
        ThreadSafeQueue<std::pair<int, NetworkMessage>> outgoingQueue;

        // Message handlers (game thread only)
        std::map<NetMessageType, std::vector<MessageHandler>> handlers;

        // ENet peer -> session ID mapping (server only)
        std::map<ENetPeer *, int> peerIdMap;

        // Network thread loops
        void ServerLoop();
        void ClientLoop();

        // Internal message dispatch
        void HandleServerMessage(ENetPeer *peer, int peerId, const NetworkMessage &msg);
        void HandleClientMessage(const NetworkMessage &msg);

        // Send raw packet
        void SendPacketTo(ENetPeer *peer, const NetworkMessage &msg, bool reliable = true);
        void BroadcastToAll(const NetworkMessage &msg, ENetPeer *exclude = nullptr, bool reliable = true);

    public:
        static NetworkManager &GetInstance();

        /**
         * @brief Initialize the network subsystem. Called by GameManager.
         */
        void init();

        /**
         * @brief Shut down networking, disconnect all peers, and join threads.
         */
        void shutdown();

        /**
         * @brief Start hosting a server on the given port.
         * @param port The port to listen on.
         * @return True if the server was started successfully.
         */
        bool StartServer(int port = kDefaultPort);

        /**
         * @brief Connect to a remote server.
         * @param ip The server IP address.
         * @param port The server port.
         * @param username The local player's username.
         * @return True if the connection was initiated successfully.
         */
        bool ConnectToServer(const std::string &ip, int port, const std::string &username);

        /**
         * @brief Disconnect from the current session.
         */
        void Disconnect();

        /**
         * @brief Process all incoming messages on the game thread.
         * Call this every frame from your scene's update method.
         */
        void ProcessMessages();

        /**
         * @brief Register a handler for a specific message type.
         * @param type The message type to handle.
         * @param handler The callback function.
         */
        void RegisterHandler(NetMessageType type, MessageHandler handler);

        /**
         * @brief Remove all handlers for a specific message type.
         */
        void ClearHandlers(NetMessageType type);

        /**
         * @brief Remove all registered handlers.
         */
        void ClearAllHandlers();

        /**
         * @brief Send a message to the server (client only).
         */
        void SendToServer(const NetworkMessage &msg);

        /**
         * @brief Send a message to a specific client (server only).
         * @param peerId The target peer's session ID.
         * @param msg The message to send.
         */
        void SendToClient(int peerId, const NetworkMessage &msg);

        /**
         * @brief Broadcast a message to all connected clients (server only).
         * @param msg The message to broadcast.
         * @param excludePeerId Optional peer ID to exclude from the broadcast (-1 = none).
         */
        void Broadcast(const NetworkMessage &msg, int excludePeerId = -1);

        /**
         * @brief Returns the current local network role.
         * @return Current network role.
         */
        NetworkRole GetRole() const;

        /**
         * @brief Returns whether the local instance is currently connected.
         * @return True when a server or client session is active.
         */
        bool IsConnected() const;

        /**
         * @brief Returns whether the local instance is acting as a server.
         * @return True when the current role is server.
         */
        bool IsServer() const;

        /**
         * @brief Returns whether the local instance is acting as a client.
         * @return True when the current role is client.
         */
        bool IsClient() const;

        /**
         * @brief Returns the local client identifier assigned by the server.
         * @return Local client identifier, or -1 when unavailable.
         */
        int GetLocalClientId() const;

        /**
         * @brief Get read-only access to the session.
         */
        const NetworkSession &GetSession() const;

        /**
         * @brief Returns the number of connected peers in the current session.
         * @return Connected peer count.
         */
        size_t GetPeerCount() const;
    };

}

#endif // COSMIC_NETWORKMANAGER_HPP
