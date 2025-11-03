#ifndef BRUTAL_CONFIG_HPP
#define BRUTAL_CONFIG_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace brutal {
// WebSocket++ client types
typedef websocketpp::client<websocketpp::config::asio_tls_client> wss;
typedef websocketpp::client<websocketpp::config::asio_client> ws;

}  // namespace brutal

#endif // BRUTAL_CONFIG_HPP
