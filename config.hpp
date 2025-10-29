#ifndef BRUTAL_CONFIG_HPP
#define BRUTAL_CONFIG_HPP

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace brutal {
    typedef websocketpp::client<websocketpp::config::asio_tls_client> wss;
    typedef websocketpp::client<websocketpp::config::asio_client> ws;
}

#endif
