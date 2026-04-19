#ifndef COSMIC_NETWORK_MESSAGE_HPP
#define COSMIC_NETWORK_MESSAGE_HPP

/**
 * @file network_message.hpp
 * @brief Declares the serialized message format used by the network subsystem.
 */

#include <nlohmann/json.hpp>
#include <string>
#include <cstdint>
#include <vector>

namespace CosmicEngine
{

    /**
     * @brief Standard message types for the network protocol.
     * Users can extend by using values >= Custom.
     */
    enum class NetMessageType : uint16_t
    {
        // Connection lifecycle
        ClientConnect       = 1,
        ClientDisconnect    = 2,
        AssignClientId      = 3,

        // Chat
        ChatMessage         = 10,

        // Game state synchronization
        PlayerState         = 20,
        SpawnObject         = 21,
        DestroyObject       = 22,
        ObjectState         = 23,
        DamageEvent         = 24,
        PlayerDeath         = 25,
        MatchEnded          = 26,

        // World snapshot
        WorldSnapshot       = 30,

        // Ping
        Ping                = 90,
        Pong                = 91,

        // User-defined messages start here
        Custom              = 100
    };

    /**
     * @brief A network message that uses JSON for payload serialization.
     * 
     * Wire format: [2 bytes type][JSON payload as UTF-8 string].
     * All messages are sent as ENet reliable packets by default.
     *
     * @par Example — creating and sending a network message
     * @code
     * CosmicEngine::NetworkMessage msg(
     *     CosmicEngine::NetMessageType::SpawnObject,
     *     {{"objectId", 42}, {"x", 10.0f}, {"y", 5.0f}, {"z", 3.0f}});
     *
     * if (NET_MN.IsServer())
     *     NET_MN.Broadcast(msg);
     * else
     *     NET_MN.SendToServer(msg);
     * @endcode
     */
    struct NetworkMessage
    {
        /** @brief Logical message type. */
        NetMessageType type;
        /** @brief JSON payload associated with the message. */
        nlohmann::json payload;

        /** @brief Creates a default custom message. */
        NetworkMessage() : type(NetMessageType::Custom) {}
        /**
         * @brief Creates a message with the provided type and payload.
         * @param type Logical message type.
         * @param payload JSON payload.
         */
        NetworkMessage(NetMessageType type, nlohmann::json payload = {})
            : type(type), payload(std::move(payload)) {}

        /**
         * @brief Serialize the message to a byte buffer for transmission.
         * @return The serialized byte buffer.
         */
        std::vector<uint8_t> Serialize() const
        {
            std::string jsonStr = payload.dump();
            std::vector<uint8_t> buffer(sizeof(uint16_t) + jsonStr.size());
            uint16_t typeVal = static_cast<uint16_t>(type);
            buffer[0] = static_cast<uint8_t>(typeVal & 0xFF);
            buffer[1] = static_cast<uint8_t>((typeVal >> 8) & 0xFF);
            std::memcpy(buffer.data() + sizeof(uint16_t), jsonStr.data(), jsonStr.size());
            return buffer;
        }

        /**
         * @brief Deserialize a message from raw packet data.
         * @param data Pointer to the raw packet data.
         * @param length Length of the data in bytes.
         * @return The deserialized NetworkMessage.
         */
        static NetworkMessage Deserialize(const uint8_t *data, size_t length)
        {
            NetworkMessage msg;
            if (length < sizeof(uint16_t))
                return msg;

            uint16_t typeVal = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
            msg.type = static_cast<NetMessageType>(typeVal);

            if (length > sizeof(uint16_t))
            {
                std::string jsonStr(reinterpret_cast<const char *>(data + sizeof(uint16_t)), length - sizeof(uint16_t));
                msg.payload = nlohmann::json::parse(jsonStr, nullptr, false);
                if (msg.payload.is_discarded())
                {
                    msg.payload = nlohmann::json::object();
                }
            }
            return msg;
        }
    };

}

#endif // COSMIC_NETWORK_MESSAGE_HPP
