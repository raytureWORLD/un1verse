#include"server/connection.hpp"
#include"io/console.hpp"


Network::Server_impl::Connection::Connection(
        Id _id, boost::asio::ip::tcp::socket&& _socket, boost::asio::io_context& _socket_io_context
):
    id(_id), 
    local_address(_socket.local_endpoint().address()),
    local_port(_socket.local_endpoint().port()),
    remote_address(_socket.remote_endpoint().address()),
    remote_port(_socket.remote_endpoint().port()),
    socket(std::move(_socket)), next_inbound_packet_length_valid(false)
{ 
    async_read();
}


std::vector<Network::Protocol::Inbound_packet> Network::Server_impl::Connection::get_and_clear_received_packets() {
    decltype(received_packets) result;

    {
        std::scoped_lock lock(received_packets_mx);

        result = std::move(received_packets);
        received_packets.clear();
    }

    return result;
}


void Network::Server_impl::Connection::send_packet(std::shared_ptr<Protocol::Outbound_packet> _packet) {
    std::scoped_lock lock(outbound_packets_mx);
    outbound_packets.emplace_back(std::move(_packet));

    async_write();
}


void Network::Server_impl::Connection::async_read() {
    std::scoped_lock lock(socket_mx);

    if(!next_inbound_packet_length_valid) {
        /* read length */

        boost::asio::async_read(
            socket,
            boost::asio::buffer(
                reinterpret_cast<std::byte*>(&next_inbound_packet_length),
                sizeof(next_inbound_packet_length)
            ),
            std::bind_front(
                &Connection::async_read_callback,
                this
            )
        );
    } else {
        /* read rest of packet */

        boost::asio::async_read(
            socket,
            boost::asio::buffer(
                next_inbound_packet_data.get() + sizeof(next_inbound_packet_length),
                next_inbound_packet_length - sizeof(next_inbound_packet_length)
            ),
            std::bind_front(
                &Connection::async_read_callback,
                this
            )
        );
    }
}


void Network::Server_impl::Connection::async_read_callback(
    boost::system::error_code const& _error,
    std::size_t _bytes_transferred
) {
    (void)_bytes_transferred;

    if(!_error) {
        if(!next_inbound_packet_length_valid) {
            /* length was read */

            next_inbound_packet_data = std::make_unique_for_overwrite<std::byte[]>(next_inbound_packet_length);
            next_inbound_packet_length_valid = true;
            async_read();

        } else {
            /* rest of packet was read */

            {
                std::scoped_lock lock(received_packets_mx);
                received_packets.emplace_back(std::move(next_inbound_packet_data), next_inbound_packet_length);
            }

            next_inbound_packet_length_valid = false;
            async_read();

        }
    } else {
        throw std::runtime_error("TODO (in read callback)");
    }
}


void Network::Server_impl::Connection::async_write() {
    /* lock on outbound_packets_mx should be held in the calling function */
    std::scoped_lock lock(socket_mx);

    std::span<std::byte const> buffer = outbound_packets.front()->get_data();

    boost::asio::async_write(
        socket,
        boost::asio::buffer(
            buffer.data(),
            buffer.size()
        ),
        std::bind_front(
            &Connection::async_write_callback,
            this
        )
    );
}

void Network::Server_impl::Connection::async_write_callback(
    boost::system::error_code const& _error,
    std::size_t _bytes_transferred
) {
    (void)_bytes_transferred;

    if(!_error) {
        {
            std::scoped_lock lock(outbound_packets_mx);
            outbound_packets.pop_front();
        }
    } else {
        throw std::runtime_error("TODO (in write callback)");
    }
}



