#ifndef BRUTAL_UTILS_HPP
#define BRUTAL_UTILS_HPP

#include <algorithm>
#include <cstdint>
#include <curl/curl.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

namespace brutal {
// leaderboard
struct leaderboard_item
{
    std::u16string nick;
    uint32_t score;
    uint16_t id;
    uint16_t rank;
};

typedef std::vector<leaderboard_item> leaderboard;

// minimap
struct minimap_object
{
    uint8_t x;
    uint8_t y;
    uint8_t r;
};

typedef std::vector<minimap_object> minimap;

// more utilities
namespace utils {

// Distance between points
inline double distance_between(double x0, double y0, double x1, double y1) {
    double dx = x1 - x0;
    double dy = y1 - y0;
    return std::sqrt(dx * dx + dy * dy);
}

// Normalize Angle
inline double normalize_angle(double angle) {
    const double PI = M_PI;
    return angle - PI * 2.0 * floor((angle + PI) / (PI * 2.0));
}

inline double angle_to_point(double x0, double y0, double x1, double y1) {
    double dx = x1 - x0;
    double dy = y1 - y0;

    return atan2(dx, dy);
}

// EnergyToRadius
inline double energy_to_radius(double energy) {
    double f = energy / 5000.0;
    f = std::min(f, 1.0);

    double add = 0.3 * std::pow(f, 1.0 / 3.0);
    double rootVal = 1.0 / (1.7 + add);

    return std::pow(energy / 100.0, rootVal) * 4.0 - 3.0;
}

inline uint16_t get_logging_id() {
    static uint16_t id = 0;
    return ++id;
}

// master
inline size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

// connect to master server
inline std::string get_server(const std::string& region, bool is_secure = false) {
    CURL* curl = curl_easy_init();

    std::string response_body;
    std::string protocol = is_secure ? "https" : "http";
    std::string url = protocol + "://master.brutal.io";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, region.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    struct curl_slist* header_list = nullptr;
    header_list = curl_slist_append(header_list, "Content-Type: text/plain");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    CURLcode curl_res = curl_easy_perform(curl);

    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (response_body == "1" || response_body == "0")
    {
        throw std::runtime_error("Server is full or link has expired");
    }

    std::string ip;
    std::string port_part;
    std::stringstream ss(response_body);
    std::getline(ss, ip, ':');
    std::getline(ss, port_part, ':');

    std::stringstream port_stream(port_part);
    std::string port_number;
    std::string room_part;
    std::getline(port_stream, port_number, '/');
    std::getline(port_stream, room_part, '/');

    std::stringstream room_stream(room_part);
    std::string room_number;
    std::getline(room_stream, room_number, '!');

    int room = std::stoi(room_number);
    int insecure_port = room + 8080;
    int secure_port = room + 8080 + 1000;

    std::stringstream full_host;
    if (is_secure)
    {
        std::replace(ip.begin(), ip.end(), '.', '-');
        full_host << "wss://" << ip << ".brutal.io:" << secure_port;
    }
    else
    {
        full_host << "ws://" << ip << ":" << insecure_port;
    }

    return full_host.str();
}

inline std::string to_u8string(const std::u16string& u16s) {
    std::string utf8;
    utf8.reserve(u16s.size());
    for (char16_t c : u16s)
    {
        utf8 += static_cast<char>(c);
    }
    return utf8;
}

struct result
{
    std::u16string nick;
    size_t offset;
};

inline result get_string(const std::vector<uint8_t>& data, size_t offset)
{
    std::u16string nick;

    while (true)
    {
        if (offset + 1 >= data.size())
        {
            throw std::out_of_range("offset out of bounds");
        }

        uint16_t value = static_cast<uint16_t>(data[offset]) | (static_cast<uint16_t>(data[offset + 1]) << 8);
        offset += 2;

        if (value == 0)
        {
            break;
        }

        nick += static_cast<char16_t>(value);
    }

    return {nick, offset};
}

} // namespace utils
} // namespace brutal

#endif // BRUTAL_UTILS_HPP
