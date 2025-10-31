#ifndef BRUTAL_CLIENT_BASE_HPP
#define BRUTAL_CLIENT_BASE_HPP

#include "handlers.hpp"
#include "utils.hpp"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <functional>
#include <chrono>
#include <random>
#include <cstdint>

namespace brutal {

// convenience
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;

using websocketpp::lib::asio::io_context;
using websocketpp::lib::asio::steady_timer;


template <typename endpoint_type>
class client_base {
public:
    typedef client_base<endpoint_type> type;
    typedef websocketpp::connection_hdl connection_hdl;
    typedef typename endpoint_type::message_ptr message_ptr;

    client_base()
        : m_nolog(false), m_autoplay(true), m_logging_id(utils::get_logging_id()), m_country("NL"),
          m_proxy(""), m_address(""), m_nick(u""), m_reconnect_after(1000) {
        m_endpoint.init_asio();
        init();
    }

    client_base(io_context& io)
        : m_nolog(false), m_autoplay(true), m_logging_id(utils::get_logging_id()), m_country("NL"),
          m_proxy(""), m_address(""), m_nick(u""), m_reconnect_after(1000) {
        m_endpoint.init_asio(&io);
        init();
    }

    void init() {
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.set_open_handler(bind(&type::on_open,this,websocketpp::lib::placeholders::_1));
        m_endpoint.set_close_handler(bind(&type::on_close,this,websocketpp::lib::placeholders::_1));
        m_endpoint.set_message_handler(bind(&type::on_message,this,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
        m_endpoint.set_fail_handler(bind(&type::on_fail,this,websocketpp::lib::placeholders::_1));
    }

    virtual void on_open(connection_hdl) = 0;
    virtual void on_close(connection_hdl) = 0;
    virtual void on_message(connection_hdl, message_ptr) = 0;
    virtual void on_fail(connection_hdl) = 0;

    void connect(std::string address, std::u16string nick = u"") {
        m_address = address;

        if(!nick.empty())
            m_nick = nick;
        else
            nick = m_nick;

        websocketpp::lib::error_code ec;
        auto con = m_endpoint.get_connection(address, ec);

        if(ec) {
            ulog(ured("socket error: " + ec.message()));
            return;
        }

        if(!m_proxy.empty()) {
            con->set_proxy(m_proxy);
        }

        if(!m_origin.empty()) {
            con->append_header("origin", m_origin);
        } else {
            con->append_header("origin", "https://brutal.io");
        }

        m_endpoint.connect(con);

        ulog(ucyan("Connecting to " + address + "..."));
    }

    bool has_connection() {
        return m_endpoint.get_con_from_hdl(m_hdl).get_state() == websocketpp::session::state::open;
    }

    bool playing() {
        return m_playing;
    }

    void send_binary(uint8_t* data, size_t size) {
        m_endpoint.send(m_hdl, data, size, websocketpp::frame::opcode::binary);
    }

    void close_connection() {
        m_endpoint.close(m_hdl, "", websocketpp::close::status::normal);
    }

    std::string get_server() {
        return utils::get_server(m_country, m_endpoint.is_secure());
    }

    // async
    void set_timeout(std::function<void(void)> callback, unsigned long time) {
        auto timer = std::make_shared<steady_timer>(get_io_context());
        timer->expires_after(std::chrono::milliseconds(time));
        timer->async_wait([timer, callback](const websocketpp::lib::error_code& ec) {
            if (!ec) {
                try {
                    callback();
                } catch (const std::exception& e) {
                    ulog(ured("set_timeout() error: " << e.what()));
                }
            }
        });
    }

    uint16_t get_player_id() {
        return m_player_id;
    }

    void run() {
        m_endpoint.run();
    }

    // handlers
    void set_open_handler(open_handler callback) {
        m_open_handler = callback;
    }

    void set_close_handler(close_handler callback) {
        m_close_handler = callback;
    }

    void set_error_handler(error_handler callback) {
        m_error_handler = callback;
    }

    void set_message_handler(message_handler callback) {
        m_message_handler = callback;
    }

    void set_enter_game_handler(enter_game_handler callback) {
        m_enter_game_handler = callback;
    }

    void set_map_handler(map_handler callback) {
        m_map_handler = callback;
    }

    void set_death_handler(death_handler callback) {
        m_death_handler = callback;
    }

    void set_kill_handler(kill_handler callback) {
        m_kill_handler = callback;
    }

    void set_king_handler(king_handler callback) {
        m_king_handler = callback;
    }

    void set_leaderboard_handler(leaderboard_handler callback) {
        m_leaderboard_handler = callback;
    }

    void set_pong_handler(pong_handler callback) {
        m_pong_handler = callback;
    }

    void set_minimap_handler(minimap_handler callback) {
        m_minimap_handler = callback;
    }
   
    void set_entity_create_handler(entity_create_handler callback) {
        m_entity_create_handler = callback;
    }

    void set_entity_update_handler(entity_update_handler callback) {
        m_entity_update_handler = callback;
    }

    void set_entity_delete_handler(entity_delete_handler callback) {
        m_entity_delete_handler = callback;
    }

    // setters
    void set_logging_id(uint16_t id) {
        m_logging_id = id;
    }

    void set_reconnect_after(unsigned long time) {
        m_reconnect_after = time;
    }

    void stop_reconnecting() {
        m_reconnect_after = 0;
    }

    void set_nick(std::u16string nick) {
        m_nick = nick;
    }

    void set_address(std::string address) {
        m_address = address;
    }

    void set_proxy(std::string proxy) {
        m_proxy = proxy;
    }

    void set_country(std::string country) {
        m_country = country;
    }

    void set_origin(std::string origin) {
        m_origin = origin;
    }

    // toggles
    void enable_autoplay() {
        m_autoplay = true;
    }

    void disable_autoplay() {
        m_autoplay = false;
    }

    void enable_logging() {
        m_nolog = false;
    }

    void disable_logging() {
        m_nolog = true;
    }

    // utils
    std::string ured(std::string text) {
        return "\033[91m" + text + "\033[0m";
    }

    std::string ugreen(std::string text) {
        return "\033[92m" + text + "\033[0m";
    }

    std::string uyellow(std::string text) {
        return "\033[93m" + text + "\033[0m";
    }

    std::string ucyan(std::string text) {
        return "\033[96m" + text + "\033[0m";
    }

    void ulog(std::string message) {
        if(m_nolog) return;
        std::cout << "[" << ucyan(std::to_string(m_logging_id)) << "] " << message << std::endl;
    }

    // getters
    endpoint_type& get_endpoint() {
        return m_endpoint;
    }

    io_context& get_io_context() {
        return m_endpoint.get_io_context();
    }

    connection_hdl get_handle() {
        return m_hdl;
    }

protected:
    endpoint_type m_endpoint;
    connection_hdl m_hdl;

    std::string m_address;
    std::u16string m_nick;
    std::string m_proxy;
    std::string m_country;
    std::string m_origin;

    uint16_t m_player_id;
    bool m_playing;
    uint16_t m_logging_id;
    bool m_nolog;
    bool m_autoplay;
    unsigned long m_reconnect_after;

    open_handler m_open_handler;
    close_handler m_close_handler;
    error_handler m_error_handler;
    message_handler m_message_handler;
    enter_game_handler m_enter_game_handler;
    map_handler m_map_handler;
    death_handler m_death_handler;
    kill_handler m_kill_handler;
    king_handler m_king_handler;
    leaderboard_handler m_leaderboard_handler;
    pong_handler m_pong_handler;
    minimap_handler m_minimap_handler;
    entity_create_handler m_entity_create_handler;
    entity_update_handler m_entity_update_handler;
    entity_delete_handler m_entity_delete_handler;
};
} // namespace brutal

#endif
