#ifndef COSMIC_NETWORK_SESSION_HPP
#define COSMIC_NETWORK_SESSION_HPP

#include <string>
#include <cstdint>
#include <map>
#include <mutex>
#include <functional>
#include <vector>

namespace CosmicEngine
{

    /**
     * @brief Represents a connected peer in the network session.
     */
    struct NetworkPeer
    {
        int id = -1;
        std::string username;

        NetworkPeer() = default;
        NetworkPeer(int id, const std::string &username = "") : id(id), username(username) {}
    };

    /**
     * @brief Manages the session state for all connected peers.
     * Thread-safe: all access to the peer map is protected by a mutex.
     */
    class NetworkSession
    {
    private:
        std::map<int, NetworkPeer> peers;
        mutable std::mutex peersMutex;
        int nextPeerId = 0;

    public:
        NetworkSession() = default;
        ~NetworkSession() = default;

        /**
         * @brief Registers a new peer and returns their assigned ID.
         */
        int AddPeer(const std::string &username = "")
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            int id = ++nextPeerId;
            peers[id] = NetworkPeer(id, username);
            return id;
        }

        /**
         * @brief Removes a peer by ID.
         */
        void RemovePeer(int id)
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            peers.erase(id);
        }

        /**
         * @brief Sets the username for a peer.
         */
        void SetPeerUsername(int id, const std::string &username)
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            auto it = peers.find(id);
            if (it != peers.end())
            {
                it->second.username = username;
            }
        }

        /**
         * @brief Gets a copy of a peer by ID. Returns false if not found.
         */
        bool GetPeer(int id, NetworkPeer &outPeer) const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            auto it = peers.find(id);
            if (it == peers.end())
                return false;
            outPeer = it->second;
            return true;
        }

        /**
         * @brief Returns true if a peer with the given ID exists.
         */
        bool HasPeer(int id) const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            return peers.count(id) > 0;
        }

        /**
         * @brief Returns a snapshot of all current peers.
         */
        std::vector<NetworkPeer> GetAllPeers() const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            std::vector<NetworkPeer> result;
            result.reserve(peers.size());
            for (const auto &[id, peer] : peers)
            {
                result.push_back(peer);
            }
            return result;
        }

        /**
         * @brief Returns the number of connected peers.
         */
        size_t GetPeerCount() const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            return peers.size();
        }

        /**
         * @brief Removes all peers.
         */
        void Clear()
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            peers.clear();
            nextPeerId = 0;
        }
    };

}

#endif // COSMIC_NETWORK_SESSION_HPP
