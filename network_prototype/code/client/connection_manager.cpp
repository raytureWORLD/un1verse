#include"client/connection_manager.hpp"


Network::Client_impl::Connection_manager::Connection_manager():
    io_context_thread(
        std::bind_front(
            &Connection_manager::io_context_thread_function,
            this
        )
    ),
    resolver(io_context)
{ }

Network::Client_impl::Connection_manager::~Connection_manager() {
    io_context.stop();
    io_context_thread.join();
}

void Network::Client_impl::Connection_manager::async_connect(
        std::string_view const& _host, std::string_view const& _service
) {
    resolver.async_resolve(
        boost::asio::string_view(_host), boost::asio::string_view(_service),
        [this](
            boost::system::error_code const& _error,
            boost::asio::ip::tcp::resolver::results_type _results
        ) {
            if(!_error) {
                boost::asio::ip::tcp::socket socket(io_context);

                try {
                    boost::asio::connect(socket, _results);

                    /* The connection id is unused */
                    /* This is safe but connection must NEVER be accessed before connect_result.success is true */
                    connection = std::make_unique<Connection>(0, std::move(socket));

                    {
                        std::scoped_lock lock(connect_result_mx);
                        connect_result.emplace(true);
                    }
                } catch(boost::system::system_error const& _exception) {
                    std::scoped_lock lock(connect_result_mx);
                    connect_result.emplace(false, _exception.what());
                }

            } else {
                std::scoped_lock lock(connect_result_mx);
                connect_result.emplace(false, _error.message());
            }

        }
    );
}


void Network::Client_impl::Connection_manager::tick() {
    if(!connection_established) {
        std::scoped_lock lock(connect_result_mx);

        if(connect_result) {
            if(connect_result->success) {
                connection_established = true;
            }

            post_event(*connect_result);
        }
    }


}

void Network::Client_impl::Connection_manager::io_context_thread_function(std::stop_token _stop_token) {
    (void)_stop_token;

    auto work = boost::asio::require(io_context.get_executor(), boost::asio::execution::outstanding_work.tracked);

    /* Exceptions thrown from handlers will be caught here */
    /* When an exception is thrown, run() must be called again */
    for(;;) {
        try {
            io_context.run();
            if(io_context.stopped()) break;
        } catch(std::exception const&) {
            throw;
        }
    }
}