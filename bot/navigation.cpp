//
// Created by egordm on 8-11-2017.
//

#include "navigation.h"

namespace bot {
    namespace navigation {

        hlt::possibly<hlt::Planet> planet_between(const hlt::Map &map, const hlt::Vector &a, const hlt::Vector &b) {
            for (const auto &planet : map.planets) {
                if (planet.pos.dist_line(a, b) < planet.radius + hlt::constants::BYPASS_PROXIMITY)
                    return std::make_pair(planet, true);
            }
            return std::make_pair(hlt::Planet(), false);
        }

        hlt::Move
        move_towards(const hlt::Map &map, const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles) {
            if (avoid_obstacles) {
                const auto obstacle = planet_between(map, ship.pos, pos);
                if (obstacle.second) {
                    const auto tangents = ship.pos.tangents(obstacle.first.pos,
                                                            obstacle.first.radius + hlt::constants::BYPASS_PROXIMITY);
                    const auto closest =
                            pos.dist(tangents.first) < pos.dist(tangents.second) ? tangents.first : tangents.second;
                    return move_towards(map, ship, closest, avoid_obstacles);
                }
            }
            auto dist = ship.pos.dist(pos);
            return hlt::Move::thrust(ship.entity_id,
                                     std::min(hlt::constants::MAX_SPEED, static_cast<const int &>(dist)),
                                     static_cast<const int>(pos.angle_between(pos)));
        }
    }
}