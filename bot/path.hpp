//
// Created by egordm on 10-11-2017.
//

#ifndef MYBOT_PATH_H
#define MYBOT_PATH_H


#include "../hlt/entities.hpp"
#include "../hlt/move.hpp"
#include "observer.h"
#include "constants.h"
#include "defines.h"

namespace bot {
    template<class T>
    class Path {
    public:
        enum AvoidDirection {
            None = 0,
            Left = -1,
            Right = 1
        };
    private:
        Observer &observer;
        const hlt::Ship &ship;
        const T &target;
        AvoidDirection avoid_direction;
        int max_corrections = 5;

        template<class U>
        hlt::Move avoid(const U &target) {
            auto radius = target.radius + ship.radius + 2; // + Epsilon

            if (ship.pos.in_radius(target.pos, radius)) {
                return move_towards(ship.pos.closest_point(target.pos, radius), false);
            }

            const auto tangents = ship.pos.tangents(target.pos, radius);
            if (avoid_direction == AvoidDirection::None) {
                if (ship.pos.dist_sq(tangents.first) < ship.pos.dist_sq(tangents.second)) {
                    avoid_direction = AvoidDirection::Left;
                } else avoid_direction = AvoidDirection::Right;
            }

            const auto closest = avoid_direction == AvoidDirection::Left ? tangents.first : tangents.second;

            max_corrections--;
            return move_towards(closest, true);
        }

        hlt::Move move_towards(const hlt::Vector &pos, bool avoid_collision) {
            if (max_corrections <= 0) return hlt::Move::noop();
            if (avoid_collision) {
                const auto pobs = obj_between(observer.getMap().planets, ship.pos, pos);
                if (pobs.second) return avoid(pobs.first);

                const auto &ships = observer.ships_around(ship.pos, constants::SHIP_AVOID_PROXIMITY, hlt::all_mask);
                const auto sobs = obj_between(ships, ship.pos, pos);
                if (sobs.second) return avoid(sobs.first);
            }

            auto speed = std::min(hlt::constants::MAX_SPEED, std::max(1, SINT(ship.pos.dist(pos))));
            return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(ship.pos.angle_between(pos))));
        }

        template<class U>
        hlt::nullable<U> obj_between(const std::vector<U> &objs, const hlt::Vector &a, const hlt::Vector &b) {
            for (const auto &obj : objs) {
                if (std::is_same<T, U>::value && obj.entity_id == target.entity_id) continue;
                if (std::is_same<hlt::Ship, U>::value && obj.entity_id == ship.entity_id) continue;

                if (obj.pos.dist_line(a, b) <= obj.radius + hlt::constants::SHIP_RADIUS) {
                    return std::make_pair(obj, true);
                }
            }
            return std::make_pair(U(), false);
        }

    public:
        Path(Observer &observer, const hlt::Ship &ship, const T &target)
                : observer(observer), ship(ship), target(target) {}

        hlt::Move move(double radius) {
            return move_towards(ship.pos.closest_point(target.pos, radius + ship.radius), true);
        }
    };
}


#endif //MYBOT_PATH_H
