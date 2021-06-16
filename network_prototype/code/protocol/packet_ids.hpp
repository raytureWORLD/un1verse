#ifndef INCLUDED_PROTOCOL_PACKET_IDS_HPP
#define INCLUDED_PROTOCOL_PACKET_IDS_HPP

#include"protocol/types.hpp"

namespace Network {
    namespace Protocol {
        namespace Packet_ids {
            enum: Packet_id {
                /* null-terminated string */
                debug_message = 1 << 0,
            };
        }
    }
}

#endif