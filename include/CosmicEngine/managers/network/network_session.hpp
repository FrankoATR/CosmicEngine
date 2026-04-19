#ifndef COSMIC_NETWORK_SESSION_HPP
#define COSMIC_NETWORK_SESSION_HPP

/**
 * @file network_session.hpp
 * @brief Declares session state containers used by the network manager.
 */

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
        /** @brief Session identifier assigned to the peer. */
        int id = -1;
        /** @brief Username associated with the peer. */
        std::string username;

        NetworkPeer() = default;
        NetworkPeer(int id, const std::string &username = "") : id(id), username(username) {}
    };

    /**
     * @brief Manages the session state for all connected peers.
     * Thread-safe: all access to the peer map is protected by a mutex.
     *
     * NetworkSession is used internally by NetworkManager to track who is
     * connected.  You do not normally interact with it directly.
     *
     * @par Example — querying peers (internal to NetworkManager)
     * @code
     * int id = session.AddPeer("Player1");
     * NetworkPeer peer;
     * if (session.GetPeer(id, peer))
     *     std::cout << peer.username << std::endl;
     * session.RemovePeer(id);
     * @endcode
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
         * @brief Registers a new peer and returns its assigned identifier.
         * @param username Optional username associated with the peer.
         * @return Assigned peer identifier.
         */
        int AddPeer(const std::string &username = "")
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            int id = ++nextPeerId;
            peers[id] = NetworkPeer(id, username);
            return id;
        }

        /**
         * @brief Removes a peer by identifier.
         * @param id Peer identifier.
         */
        void RemovePeer(int id)
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            peers.erase(id);
        }

        /**
         * @brief Sets the username associated with a peer.
         * @param id Peer identifier.
         * @param username New username.
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
         * @brief Gets a copy of a peer by identifier.
         * @param id Peer identifier.
         * @param outPeer Output peer copy when one is found.
         * @return True when the peer exists.
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
         * @brief Returns whether a peer with the provided identifier exists.
         * @param id Peer identifier.
         * @return True when the peer exists.
         */
        bool HasPeer(int id) const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            return peers.count(id) > 0;
        }

        /**
         * @brief Returns a snapshot of all connected peers.
         * @return Copy of the peer list.
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
         * @return Peer count.
         */
        size_t GetPeerCount() const
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            return peers.size();
        }

        /** @brief Removes all peers and resets the next peer identifier. */
        void Clear()
        {
            std::lock_guard<std::mutex> lock(peersMutex);
            peers.clear();
            nextPeerId = 0;
        }
    };

}

#endif // COSMIC_NETWORK_SESSION_HPP
