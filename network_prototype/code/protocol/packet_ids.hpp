#ifndef INCLUDED_PROTOCOL_PACKET_IDS_HPP
#define INCLUDED_PROTOCOL_PACKET_IDS_HPP

#include"protocol/types.hpp"

namespace Network {
    namespace Protocol {
        namespace Packet_ids {
            enum: Packet_id {
                /* string: message */
                debug_message = 1 << 0,

                /* uint8: status (0 - connected, 1 - disconnected)
                connection_id: id of the connection */
                connection_state_change = 1 << 1,
            };
        }
    }
}

#endif