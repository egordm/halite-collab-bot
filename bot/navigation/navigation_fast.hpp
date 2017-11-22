//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_HPP_H
#define HALITE_ORION_NAVIGATION_HPP_H

#include <sstream>
#include <algorithm>
#include <math.hpp>
#include <functional>
#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include "../utils.h"
#include "../sorting.h"
#include "collision_checking.hpp"

using namespace std::placeholders;

namespace bot {
    namespace navigation {
        struct MovePromise {
            const hlt::EntityId ship_id;
            const hlt::MoveType type;
            hlt::Vector target;
            hlt::EntityId dock_to;
            hlt::Vector velocity;

            MovePromise(const hlt::EntityId ship_id, const hlt::MoveType type, const hlt::EntityId dock_to,
                        const hlt::Vector &velocity, const hlt::Vector &target)
                    : ship_id(ship_id), type(type), dock_to(dock_to), velocity(velocity), target(target) {}

            hlt::Move produce() const {
                switch (type) {
                    case hlt::MoveType::Noop:
                        return hlt::Move::noop();
                    case hlt::MoveType::Thrust: {
                        const double angle = std::round(hlt::rad_to_deg(velocity.angle()));
                        const auto speed = std::min(7, static_cast<const int>(std::round(velocity.length())));
                        return hlt::Move::thrust(ship_id, speed, static_cast<const int>(angle));
                    }
                    case hlt::MoveType::Dock:
                        return hlt::Move::dock(ship_id, dock_to);
                    case hlt::MoveType::Undock:
                        return hlt::Move::undock(ship_id);
                }
            }

            static MovePromise noop() {
                return {0, hlt::MoveType::Noop, 0, hlt::Vector(), hlt::Vector()};
            }

            static MovePromise dock(const hlt::EntityId ship_id, const hlt::EntityId dock_to) {
                return {ship_id, hlt::MoveType::Dock, dock_to, hlt::Vector(), hlt::Vector()};
            }

            static MovePromise undock(const hlt::EntityId ship_id) {
                return {ship_id, hlt::MoveType::Undock, 0, hlt::Vector(), hlt::Vector()};
            }

            static MovePromise
            thrust(const hlt::EntityId ship_id, const hlt::Vector &velocity, const hlt::Vector &target) {
                return {ship_id, hlt::MoveType::Thrust, 0, velocity, target};
            }
        };


        constexpr double BYPASS_MARGIN = 0.2;

        // TODO: hitmap for path planning
        class FastPath {
        protected:
            Observer &observer;
            std::vector<hlt::EntityIdentifier> ignore_list{};
            double target_speed;

            /**
             * TODO: use a vector of aversions
             * @param a
             * @param b
             * @param aversions
             */
            virtual hlt::Vector pick_shortest_route(const hlt::Vector &a, const hlt::Vector &b,
                                                    const std::vector<hlt::Vector> &aversions) {
                hlt::Vector ret = *std::min_element(aversions.begin(), aversions.end(),
                                                    [&a, &b](const hlt::Vector &e1, const hlt::Vector &e2) {
                                                        return !e1.isnan() &&
                                                               a.dist(e1) + b.dist(e1) < a.dist(e2) + b.dist(e2);
                                                    });

                return ret.isnan() ? b : ret;
            }

            virtual void get_obstacles(const hlt::Vector &a, const hlt::Vector &b,
                                       std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) {
                check_collisions(observer.get_planets(), a, b, ignore_list, obst);
                auto ships = observer.get_ships();
                utils::erase_if(ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
                check_collisions(ships, a, b, ignore_list, obst);
            }

            virtual hlt::Vector calc_avoid_point(const hlt::Vector &a, const hlt::Vector &b,
                                                 const std::pair<hlt::Entity *, hlt::Vector> &obstacle,
                                                 const bool first_tangent) {
                if (a.in_radius(obstacle.second,
                                obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN)) {
                    return a.closest_point(obstacle.second,
                                           obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);
                }

                auto tangents = a.tangents(obstacle.second,
                                           obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

                // Angle correction since converting it to int goes kind of bad
                return math::compensate_shitint(obstacle.second, a, first_tangent ? tangents.first : tangents.second);
            }

            virtual hlt::Vector
            correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_left, unsigned int max_corrections) {
                if (max_corrections == 0) return hlt::Vector::NAN_VEC;

                std::vector<std::pair<hlt::Entity *, hlt::Vector>> obst;
                get_obstacles(a, b, obst);
                if (obst.empty()) return b;

                auto closest_obst = *std::min_element(obst.begin(), obst.end(),
                                                      std::bind(sorting::sort_obst_by_distance, a, _1, _2));

                return correct_route(a, calc_avoid_point(a, b, closest_obst, correct_left), correct_left,
                                     max_corrections - 1);
            }

        public:
            FastPath(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore_list, double target_speed)
                    : observer(observer), ignore_list(ignore_list), target_speed(target_speed) {}

            FastPath(Observer &observer, double target_speed)
                    : observer(observer), ignore_list(ignore_list), target_speed(target_speed) {}

            MovePromise navigate(const hlt::Ship *ship, const hlt::Vector &target) {
                ignore_list.push_back(ship->identify());

                auto aversions = {
                        correct_route(ship->pos, target, true, 30),
                        correct_route(ship->pos, target, false, 30)
                };

                hlt::Vector corrected_target = pick_shortest_route(ship->pos, target, aversions);


                auto speed = std::min(ship->pos.dist(target), target_speed);
                auto vel = math::resize_line(ship->pos, corrected_target, speed) - ship->pos;
                return MovePromise::thrust(ship->entity_id, vel, target);
            }
        };
    }
}

#endif //HALITE_ORION_NAVIGATION_HPP_H
