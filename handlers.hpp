#ifndef BRUTAL_HANDLERS_HPP
#define BRUTAL_HANDLERS_HPP

#include "utils.hpp"

#include <string>
#include <cstdint>
#include <vector>
#include <system_error>
#include <functional>

namespace brutal {
// forward
struct entity;

// event handlers
/*
    websocket open
*/
typedef std::function<void(void)>                        open_handler;

/*
    websocket close
*/
typedef std::function<void(void)>                        close_handler;

/*
    websocket error
*/
typedef std::function<void(std::error_code&)>            error_handler;

/*
    websocket message
*/
typedef std::function<void(std::vector<uint8_t>&)>       message_handler;

/*
    received id
*/
typedef std::function<void(uint16_t)>                    entered_game_handler;

/*
    received map config
*/
typedef std::function<void(uint8_t,float,float)>         map_handler;

/*
    deleted by someone
*/
typedef std::function<void(uint16_t,std::u16string)>     death_handler;

/*
    deleted someone
*/
typedef std::function<void(uint16_t,std::u16string)>     kill_handler;

/*
    got king packet
*/
typedef std::function<void(uint16_t,float,float)>        king_handler;

/*
    received leaderboard stats
*/
typedef std::function<void(leaderboard&)>                leaderboard_handler;

/*
    pong by server
*/
typedef std::function<void(void)>                        pong_handler;

/*
    receieved minimap packet
*/
typedef std::function<void(minimap&)>                    minimap_handler;

/*
    entity update
*/
typedef std::function<void(entity*)>                     entity_update_handler;

/*
    entity create
*/
typedef std::function<void(entity*)>                     entity_create_handler;

/*
    entity delete
*/
typedef std::function<void(entity*)>                     entity_delete_handler;

} // namespace brutal


#endif
