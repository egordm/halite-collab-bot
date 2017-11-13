#pragma once

#include <memory>
#include "map.hpp"
#include "types.hpp"
#include "entities.hpp"

namespace hlt {
    class Map {
    public:
        int map_width, map_height;

        entity_map<std::shared_ptr<Ship>> ships{};
        std::unordered_map<PlayerId, std::vector<std::shared_ptr<Ship>>> players{};

        entity_map<std::shared_ptr<Planet>> planets;

        Map(int width, int height);

        const std::shared_ptr<Ship> get_ship(const EntityId ship_id) const {
            return ships.at(ship_id);
        }

        const std::shared_ptr<Planet> get_planet(const EntityId planet_id) const {
            return planets.at(planet_id);
        }
    };
}
