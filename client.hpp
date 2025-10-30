#ifndef BRUTAL_CLIENT_HPP
#define BRUTAL_CLIENT_HPP

#include "client_base.hpp"
#include "opcodes.hpp"
#include "config.hpp"
#include "utils.hpp"

#include "entity.hpp"
#include "atom.hpp"
#include "ship.hpp"
#include "boundary.hpp"
#include "energy.hpp"
#include "collider.hpp"
#include "red_flail_powerup.hpp"
#include "tri.hpp"

#include <cstring>
#include <vector>
#include <stdexcept>
#include <unordered_map>

namespace brutal {

template <typename endpoint_type>
class client : public client_base<endpoint_type> {
public:
    using typename client_base<endpoint_type>::connection_hdl;
    using typename client_base<endpoint_type>::message_ptr;

    std::unordered_map<uint16_t, entity*> entities;

    client() : local_player(nullptr), client_base<endpoint_type>() {}

    client(websocketpp::lib::asio::io_context& io) : local_player(nullptr), client_base<endpoint_type>(io) {}

    void on_open(connection_hdl hdl) override {
        this->ulog(this->ugreen("Connected!"));
        this->m_hdl = hdl;

        if (this->m_open_handler) {
            this->m_open_handler();
        }

        this->ping();
        this->send_hello();

        if (this->m_autoplay) {
            this->send_nick(this->m_nick);
        }
    }

    void on_close(connection_hdl) override {
        this->ulog(this->yellow("disconnected."));

        if (this->m_close_handler) {
            this->m_close_handler();
        }

        if (!this->m_reconnect_after)
            return;

        this->set_timeout([this]() {
            this->connect(this->m_address, this->m_nick);
        }, this->m_reconnect_after);
    }

    void on_fail(connection_hdl hdl) override {
        auto con = this->m_endpoint.get_con_from_hdl(hdl);
        this->ulog(this->ured(con->get_ec().message()));

        if (this->m_error_handler) {
            websocketpp::lib::error_code ec = con->get_ec();
            this->m_error_handler(ec);
        }
    }

    void on_message(connection_hdl, message_ptr msg) override {
        std::vector<uint8_t> data(msg->get_payload().begin(), msg->get_payload().end());
        if(data.empty()) return;

        if(this->m_message_handler) {
            this->m_message_handler(data);
        }

        switch (data[0]) {
            case opcodes::server::pong:
                this->ulog(this->ucyan("pong!"));
                if(this->m_pong_handler) {
                    this->m_pong_handler();
                }
                break;

            case opcodes::server::entered_game:
                process_id(data);
                break;

            case opcodes::server::entity_info_v1:
            case opcodes::server::entity_info_v2:
                update_entities(data, data[0]);
                break;

            case opcodes::server::map_config:
                process_map(data);
                break;

            case opcodes::server::events:
                process_events(data, data[0]);
                break;

            case opcodes::server::leaderboard_v1:
            case opcodes::server::leaderboard_v2:
                process_leaderboard(data, data[0]);
                break;

            case opcodes::server::minimap:
                process_minimap(data);
                break;

            default:
                this->ulog(this->ured("Unknown opcode: " + std::to_string(data[0])));
                break;
        }
    }

    void update_entities(std::vector<uint8_t>& data, uint8_t op) {
        size_t offset = 1; // Skip opcode

        while(true) {
            uint16_t entity_id;
            std::memcpy(&entity_id, data.data() + offset, 2);
            offset += 2;

            if(entity_id == 0x0) {
                if(offset < data.size()) {
                    uint16_t king_id;
                    std::memcpy(&king_id, data.data() + offset, 2);
                    offset += 2;

                    if(king_id > 0) {
                        if (offset + 8 > data.size()) break;

                        float king_x, king_y;
                        std::memcpy(&king_x, data.data() + offset, 4);
                        offset += 4;
                        std::memcpy(&king_y, data.data() + offset, 4);
                        offset += 4;

                        king_x *= 10;
                        king_y *= -10;

                        if(this->m_king_handler) {
                            this->m_king_handler(king_id, king_x, king_y);
                        }
                    }
                }
                break;
            }

            if (offset >= data.size()) break;

            uint8_t flags = data[offset];
            offset += 1;

            entity* e = nullptr;

            switch(flags) {
                case 0x0: // Partial
                {
                    e = entities[entity_id];
                    if(e) {
                        offset = e->update_network(data, offset, false, op);
                        
		                if(this->m_entity_update_handler) {
			                this->m_entity_update_handler(e);
		                }
                        
                        if(e->m_update_handler) {
                            e->m_update_handler(e);
                        }
                    } else {
                        this->ulog(this->ured("Entity with id: " + std::to_string(entity_id) + " not found"));
                    }
                    break;
                }

                case 0x1: // Full
                {
                    uint8_t entity_type = data[offset++];
                    uint8_t entity_sub_type = data[offset++];

                    auto res = utils::get_string(data, offset);
                    std::u16string nick = res.nick;
                    offset = res.offset;

                    // Create entity according to type and subtype
                    switch(entity_type) {
                        case opcodes::entities::player:
                            e = new ship();
                            break;
                        case opcodes::entities::item:
                            switch (entity_sub_type) {
                                case opcodes::entities::item_atom: e = new atom(); break;
                                case opcodes::entities::item_energy: e = new energy(); break;
                                case opcodes::entities::item_tri_plus:
                                case opcodes::entities::item_tri_minus:
                                    e = new tri(entity_sub_type);
                                    break;
                                case opcodes::entities::item_redflail: e = new red_flail_powerup(); break;
                            }
                            break;
                        case opcodes::entities::collider:
                            if (entity_sub_type == opcodes::entities::boundary)
                                e = new boundary();
                            else
                                e = new collider(entity_sub_type);
                            break;
                        default:
                            this->ulog(this->ured(
                                "ERROR: Creating unknown entity type: " + std::to_string(entity_type) +
                                " Subtype: " + std::to_string(entity_sub_type)
                            ));
                            break;
                    }

                    if(e) {
                        e->nick = nick;
                        e->id = entity_id;
                        entities[entity_id] = e;
                        offset = e->update_network(data, offset, true, op);

                        bool is_self = (entity_id == this->m_player_id);

                        if(is_self) {
                            local_player = reinterpret_cast<ship*>(e);
                        }

			            if(this->m_entity_create_handler) {
		       	            this->m_entity_create_handler(e);
			            }

                        if(e->m_create_handler) {
                            e->m_create_handler(e);
                        }
                    } else {
                        this->ulog(this->ured(
                            "Unable to create entity. Entity type: " + std::to_string(entity_type)
                        ));
                    }
                    break;
                }

                case 0x2: // Delete
                {
                    uint16_t killed_by_id;
                    std::memcpy(&killed_by_id, data.data() + offset, 2);
                    offset += 2;

                    uint8_t kill_reason = data[offset++];
                    e = entities[entity_id];
                    if(e) {
                        e->kill_reason = kill_reason;
                        e->killed_by_id = killed_by_id;
                        offset = e->delete_network(data, offset);
						
						if(this->m_entity_delete_handler) {
							this->m_entity_delete_handler(e);
						}

                        if(e->m_delete_handler) {
                            e->m_delete_handler(e);
                        }

						delete e;
                        entities.erase(entity_id);
                    } else {
                        this->ulog(this->ured("ERROR: Entity does not exist: " + std::to_string(entity_id)));
                    }
                    break;
                }

                default:
                    this->ulog(this->ured("Invalid entity flag"));
                    break;
            }
        }
    }

    void process_map(std::vector<uint8_t>& data) {
        int offset = 1;
        float cfg_arena_width, cfg_arena_height;

        std::memcpy(&cfg_arena_width, &data[offset], 4);
        offset += 4;

        std::memcpy(&cfg_arena_height, &data[offset], 4);
        offset += 4;

        uint8_t map_version = data[offset];
        float arena_width = cfg_arena_width * 10;
        float arena_height = cfg_arena_height * 10;

        this->ulog(this->ucyan("received map config"));

        if(this->m_map_handler) {
            this->m_map_handler(map_version, arena_width, arena_height);
        }
    }

    void process_minimap(std::vector<uint8_t>& data) {
		int offset = 1; // Skip opcode
		uint16_t count;
        std::memcpy(&count, data.data() + offset, 2);
		offset += 2;
        
		minimap mapinfo;
		for(int i = 0; i < count; i++) {
			uint8_t x = data[offset++];
			uint8_t y = data[offset++];
			uint8_t r = data[offset++];
			mapinfo.push_back({x, 256-y, r});
		}
        
		if(this->m_minimap_handler) {
            this->m_minimap_handler(mapinfo);
        }
    }

    void process_events(std::vector<uint8_t>& data, uint8_t) {
        int offset = 1;

        while(true) {
            uint8_t _byte;
            std::memcpy(&_byte, data.data() + offset, 1);
            offset += 1;

            if(_byte == 0x0) break;

            switch(_byte) {
                case opcodes::events::did_kill:
                {
                    uint16_t id;
                    std::memcpy(&id, data.data() + offset, 2);
                    offset += 2;

                    auto res = utils::get_string(data, offset);
                    std::u16string nick = res.nick;
                    offset = res.offset;

                    if(this->m_kill_handler) {
                        this->m_kill_handler(id, nick);
                    }

                    this->ulog(this->ucyan("deleted " + std::to_string(id)));
                }
                break;

                case opcodes::events::was_killed:
                {
                    uint16_t id;
                    std::memcpy(&id, data.data() + offset, 2);
                    offset += 2;

                    auto res = utils::get_string(data, offset);
                    std::u16string nick = res.nick;
                    offset = res.offset;

                    if(this->m_death_handler) {
                        this->m_death_handler(id, nick);
                    }

                    if(this->m_autoplay) {
                        this->send_nick();
                    }

                    this->ulog(this->uyellow("deleted by " + std::to_string(id)));
                }
                break;

                default:
                    this->ulog(this->ured("error processing event"));
                break;
            }
        }
    }

    void process_id(std::vector<uint8_t>& data) {
        uint32_t id;
        std::memcpy(&id, data.data() + 1, 4);

        this->ulog(this->ugreen("Entered game! id: " + std::to_string(id)));

        if(this->m_enter_game_handler) {
            this->m_enter_game_handler(id);
        }

        this->m_player_id = id;
    }

    void process_leaderboard(const std::vector<uint8_t>& data, uint8_t op) {
        size_t offset = 1;
        bool containsData = false;
        leaderboard lb;

        while(true) {
            if (offset + 1 >= data.size()) break;
            uint16_t id = data[offset] | (data[offset + 1] << 8);
            offset += 2;

            if(id == 0x0) break;

            containsData = true;

            uint32_t score;
            if(op == 1) {
                score = data[offset] | (data[offset + 1] << 8);
                offset += 2;
            } else {
                score = data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24);
                offset += 4;
            }

            auto res = utils::get_string(data, offset);
            std::u16string nick = res.nick;
            offset = res.offset;

            lb.push_back({nick, score, id, 0});
        }

        if(offset + 1 < data.size()) {
            uint16_t id = data[offset] | (data[offset + 1] << 8);
            offset += 2;

            if (id > 0) {
                uint32_t score;
                if (op == 1) {
                    score = data[offset] | (data[offset + 1] << 8);
                    offset += 2;
                } else {
                    score = data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24);
                    offset += 4;
                }

                uint16_t rank = data[offset] | (data[offset + 1] << 8);
                offset += 2;

                lb.push_back({u"", score, id, rank});
            }
        }

        if(containsData && this->m_leaderboard_handler) {
            this->m_leaderboard_handler(lb);
        }
    }

    void ping() {
        this->ulog(this->ucyan("pinging!"));
        uint8_t buf[] = {opcodes::client::ping};
        this->send_binary(buf, 1);
    }

    void send_hello() {
        this->ulog(this->ucyan("hello!"));
        uint8_t buf[] = {1, 55, 2, 131, 1};
        this->send_binary(buf, sizeof(buf));
    }

    void send_nick() {
        send_nick(this->m_nick);
    }

    void send_nick(const std::u16string& nick) {
        this->ulog(this->ucyan("sending nick..."));

        std::vector<uint8_t> buf(1 + nick.length() * 2 + 3, 0);
        buf[0] = opcodes::client::enter_game;
        std::memcpy(buf.data() + 1, nick.data(), nick.length() * 2);

        this->send_binary(buf.data(), buf.size());
    }

    void leave() {
        this->ulog(this->ucyan("leaving..."));
        uint8_t buf[] = {opcodes::client::leave_game};
        this->send_binary(buf, sizeof(buf));
    }

    void send_input(double angle, uint8_t throttle) {
        uint8_t buf[10];
        buf[0] = opcodes::client::input;
        std::memcpy(buf + 1, &angle, sizeof(double));
        buf[9] = throttle;
        this->send_binary(buf, sizeof(buf));
    }

    void send_click(uint8_t shooting) {
        uint8_t buf[] = {opcodes::client::click, shooting};
        this->send_binary(buf, sizeof(buf));
    }

    void click_once() {
        send_click(true);
        this->set_timeout([this]() {
            send_click(false);
        }, 135);
    }

    ship& player() {
		return *local_player;
	}

private:
    ship* local_player;
};
} // namespace brutal

#endif
