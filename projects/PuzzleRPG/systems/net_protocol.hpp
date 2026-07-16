#ifndef PUZZLERPG_NET_PROTOCOL_HPP
#define PUZZLERPG_NET_PROTOCOL_HPP

#include <CosmicEngine/managers/network/network_message.hpp>

// Mensajes propios del juego, por encima de NetMessageType::Custom (=100).
// El servidor es autoritativo sobre el estado del mundo y replica cambios a
// los clientes. Los clientes solo envian intenciones (ej. "quiero moverme").
namespace PuzzleRPG
{
    namespace Net
    {
        // Identificadores de mensaje. Se asignan empezando en 100.
        constexpr CosmicEngine::NetMessageType Msg_LevelLoad      = static_cast<CosmicEngine::NetMessageType>(100);
        constexpr CosmicEngine::NetMessageType Msg_PlayerJoin     = static_cast<CosmicEngine::NetMessageType>(101);
        constexpr CosmicEngine::NetMessageType Msg_PlayerLeave    = static_cast<CosmicEngine::NetMessageType>(102);
        constexpr CosmicEngine::NetMessageType Msg_PlayerIntent   = static_cast<CosmicEngine::NetMessageType>(103);
        constexpr CosmicEngine::NetMessageType Msg_PlayerState    = static_cast<CosmicEngine::NetMessageType>(104);
        constexpr CosmicEngine::NetMessageType Msg_ObjectMoved    = static_cast<CosmicEngine::NetMessageType>(105);
        constexpr CosmicEngine::NetMessageType Msg_ObjectPicked   = static_cast<CosmicEngine::NetMessageType>(106);
        constexpr CosmicEngine::NetMessageType Msg_ObjectPlaced   = static_cast<CosmicEngine::NetMessageType>(107);
        constexpr CosmicEngine::NetMessageType Msg_PlateState     = static_cast<CosmicEngine::NetMessageType>(108);
        constexpr CosmicEngine::NetMessageType Msg_ButtonState    = static_cast<CosmicEngine::NetMessageType>(109);
        constexpr CosmicEngine::NetMessageType Msg_DoorState      = static_cast<CosmicEngine::NetMessageType>(110);
        constexpr CosmicEngine::NetMessageType Msg_Dialog         = static_cast<CosmicEngine::NetMessageType>(111);
        constexpr CosmicEngine::NetMessageType Msg_DialogChoice   = static_cast<CosmicEngine::NetMessageType>(112);
        constexpr CosmicEngine::NetMessageType Msg_GameEnd        = static_cast<CosmicEngine::NetMessageType>(113);
        constexpr CosmicEngine::NetMessageType Msg_TeleporterStart = static_cast<CosmicEngine::NetMessageType>(114);
        constexpr CosmicEngine::NetMessageType Msg_WorldState      = static_cast<CosmicEngine::NetMessageType>(115);

        // Tipos de intencion que el cliente envia al servidor.
        enum IntentType
        {
            Intent_Move        = 1, // payload: dir (0=down,1=up,2=left,3=right)
            Intent_Interact    = 2, // payload: vacio (intenta usar la casilla de enfrente)
            Intent_Pickup      = 3,
            Intent_Place       = 4,
            Intent_DialogReply = 5  // payload: dialogId, choice (true/false)
        };

        // Direcciones cardinales en formato discreto.
        enum Facing
        {
            Face_Down  = 0,
            Face_Up    = 1,
            Face_Left  = 2,
            Face_Right = 3
        };
    }
}

#endif
