#include"server/connection.hpp"
#include"io/console.hpp"


Network::Server_impl::Connection::Connection(Id _id, boost::asio::ip::tcp::socket&& _socket):
    id(_id), 
    local_address(_socket.local_endpoint().address()),
    local_port(_socket.local_endpoint().port()),
    remote_address(_socket.remote_endpoint().address()),
    remote_port(_socket.remote_endpoint().port()),
    socket(std::move(_socket)), 
    next_inbound_packet_length_valid(false),
    is_dead_(false)
{ }

Network::Server_impl::Connection::~Connection() {
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    /* This cancels all outstanding async operations, even though none should be left by now, because
    the destructor is called when the last shared_ptr is destroyed in the last callback to be executed */
    socket.close(); 
}

std::vector<Network::Protocol::Inbound_packet> Network::Server_impl::Connection::get_and_clear_received_packets() {
    decltype(received_packets) result;

    if(is_dead_.load()) return result;

    std::call_once(
        init_async_read_once_flag, 
        &Connection::async_read, 
        this
    );

    {
        std::scoped_lock lock(received_packets_mx);

        result = std::move(received_packets);
        received_packets.clear();
    }

    return result;
}


void Network::Server_impl::Connection::send_packet(std::shared_ptr<Protocol::Outbound_packet> _packet) {
    if(is_dead_.load()) return;

    std::scoped_lock lock(outbound_packets_mx);
    outbound_packets.emplace_back(std::move(_packet));

    /* lock on outbound packets is held before entering this call */
    if(outbound_packets.size() == 1) async_write();
}

bool Network::Server_impl::Connection::is_dead() const {
    return is_dead_.load();
}

std::string const& Network::Server_impl::Connection::get_dead_reason() const {
    return dead_reason;
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
                this,
                shared_from_this()
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
                this,
                shared_from_this()
            )
        );
    }
}


void Network::Server_impl::Connection::async_read_callback(
    std::shared_ptr<Connection>,
    boost::system::error_code const& _error,
    std::size_t _bytes_transferred
) {
    (void)_bytes_transferred;

    if(is_dead_.load()) return;

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
        dead_reason = _error.message();
        is_dead_.store(true);
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
            this,
            shared_from_this()
        )
    );
}

void Network::Server_impl::Connection::async_write_callback(
    std::shared_ptr<Connection>,
    boost::system::error_code const& _error,
    std::size_t _bytes_transferred
) {
    (void)_bytes_transferred;

    if(is_dead_.load()) return;

    if(!_error) {
        std::scoped_lock lock(outbound_packets_mx);
        outbound_packets.pop_front();

        /* lock on outbound packets is held before entering this call */
        if(!outbound_packets.empty()) async_write();
    } else {
        dead_reason = _error.message();
        is_dead_.store(true);
    }
}



