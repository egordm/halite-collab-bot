//
// Created by egordm on 21-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_THOROUGH_H
#define HALITE_ORION_NAVIGATION_THOROUGH_H

#include <observer.h>
#include <math.hpp>
#include <sorting.h>
#include <utils.h>
#include <sstream>
#include "navigation_fast.hpp"
#include "collision_checking.hpp"

namespace bot {
    namespace navigation {

        constexpr double SHIP_COLLISION_CHECK_RADIUS = hlt::constants::MAX_SPEED + 2 * hlt::constants::SHIP_RADIUS;
        constexpr double SHIP_BYPASS_MARGIN = 0.2;

        class NavigationCorrector : public FastPath {

        protected:
            void
            get_colliding_ships(const hlt::Vector &a, const hlt::Vector &b,
                                std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) {
                FastPath::get_obstacles(a, b, obst);
                // Ship trajectory correct
                auto vel = math::resize_line(a, b, target_speed) - a;
                const auto &others = observer.get_ships(a + vel, SHIP_COLLISION_CHECK_RADIUS, hlt::all_mask);

                for (const auto &other : others) {
                    if (std::find(ignore_list.begin(), ignore_list.end(), other->identify()) !=
                        ignore_list.end())
                        continue;
                    bool collides;
                    auto colt = time_of_closest_approach(a, other->pos, vel, other->vel,
                                                         hlt::constants::SHIP_RADIUS + SHIP_BYPASS_MARGIN,
                                                         other->radius, collides);
                    if (!collides) continue;

                    auto colp = point_of_collision(a, other->pos, vel, other->vel, hlt::constants::SHIP_RADIUS,
                                                   other->radius, colt);
                    obst.emplace_back(other.get(), colp);

                    std::stringstream ss;
                    ss << "Colliding with ship: " << other->entity_id << " Pos: " << other->pos << " Colp: " << colp;
                    hlt::Log::log(ss.str());
                }
            }

            void get_obstacles(const hlt::Vector &a, const hlt::Vector &b,
                               std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) override {
                FastPath::get_obstacles(a, b, obst);
            }

            hlt::Vector calc_ship_avoid_point(const hlt::Vector &a, const hlt::Vector &b, const hlt::Ship *obstacle,
                                              const hlt::Vector &colp, bool correct_left) {


                if(do_trajectoies_cross(a, obstacle->pos, b, obstacle->pos + obstacle->vel)) {
                    const auto speed = a.dist(colp) - (hlt::constants::SHIP_RADIUS * 2 + SHIP_BYPASS_MARGIN);
                    return math::resize_line(a, b, speed);
                }

            }

            hlt::Vector correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_left,
                                      unsigned int max_corrections) override {
                if (max_corrections == 0) return hlt::Vector::NAN_VEC;

                std::vector<std::pair<hlt::Entity *, hlt::Vector>> obst;
                get_obstacles(a, b, obst);
                get_colliding_ships(a, b, obst); // TODO better to use in get_obstacles
                if (obst.empty()) return b;

                auto closest_obst = *std::min_element(obst.begin(), obst.end(),
                                                      std::bind(sorting::sort_obst_by_distance, a, _1, _2));

                // A ship is in my way
                if (closest_obst.first->get_type() == hlt::EntityType::Ship &&
                    dynamic_cast<hlt::Ship *>(closest_obst.first)->docking_status == hlt::ShipDockingStatus::Undocked) {

                }

                return correct_route(a, calc_avoid_point(a, b, closest_obst, correct_left), correct_left,
                                     max_corrections - 1);
            }

            void correct(MovePromise &move_promise) {
                const auto &ship = observer.get_ship(move_promise.ship_id);
                ignore_list = {ship->identify()};
                target_speed = move_promise.velocity.length();

                auto aversions = { // TODO: some ship aversions only one best path
                        correct_route(ship->pos, ship->pos +ship->vel, true, 30),
                        correct_route(ship->pos, ship->pos +ship->vel, false, 30)
                };

                hlt::Vector corrected_target = pick_shortest_route(ship->pos, corrected_target, aversions);
                move_promise.velocity = corrected_target - ship->pos;
                move_promise.target = corrected_target;

                ship->vel = move_promise.velocity;

            }

        public:
            explicit NavigationCorrector(Observer &observer)
                    : FastPath(observer, hlt::constants::MAX_SPEED) {}

            void correct_moves(std::vector<MovePromise> &promises) {
                for (auto &promise : promises) correct(promise);
            }

        };
    }
}

#endif //HALITE_ORION_NAVIGATION_THOROUGH_H
