#pragma once

#include <sstream>
#include <iostream>

#include "map.hpp"
#include "entities.hpp"

namespace hlt {
    namespace in {
        static std::string get_string() {
            std::string result;
            std::getline(std::cin, result);
            return result;
        }

        static std::shared_ptr<Ship> parse_ship(std::stringstream &iss, PlayerId owner_id, const Map *map);

        static std::shared_ptr<Planet> parse_planet(std::istream &iss, const Map *map);

        static void update_map(const std::string &input, Map *map);

        void setup(const std::string &bot_name, int map_width, int map_height);

        void update_map(Map *map);
    }
}
