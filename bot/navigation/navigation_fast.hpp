//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_HPP_H
#define HALITE_ORION_NAVIGATION_HPP_H

#include <sstream>
#include <algorithm>
#include <math.hpp>
#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include "../utils.h"
#include "../sorting.h"
#include "collision_checking.hpp"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		constexpr double BYPASS_MARGIN = 0.2;
		constexpr double SHIP_COLLISION_CHECK_RADIUS = hlt::constants::MAX_SPEED + 2 * hlt::constants::SHIP_RADIUS;

		// TODO: hitmap for path planning
		class FastPath {
		protected:
			Observer &observer;
			std::vector<hlt::EntityIdentifier> &ignore_list;
			double target_speed;

			virtual void get_obstacles(const hlt::Vector &a, const hlt::Vector &b, std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) {
				check_collisions(observer.get_planets(), a, b, ignore_list, obst);
				auto ships = observer.get_ships();
				utils::erase_if(ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
				check_collisions(ships, a, b, ignore_list, obst);
			}

			virtual hlt::Vector calc_avoid_point(const hlt::Vector &a, const hlt::Vector &b, const std::pair<hlt::Entity *, hlt::Vector> &obstacle,
			                             const bool first_tangent) {
				if (a.in_radius(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN)) {
					return a.closest_point(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);
				}

				auto tangents = a.tangents(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

				// Angle correction since converting it to int goes kind of bad
				return math::compensate_shitint(obstacle.second, a, first_tangent ? tangents.first : tangents.second);
			}

			virtual hlt::nullable<hlt::Vector>
			correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_left, unsigned int max_corrections) {
				if (max_corrections == 0) return {b, false};

				std::vector<std::pair<hlt::Entity *, hlt::Vector>> obst;
				get_obstacles(a, b, obst);
				if (obst.empty()) return {b, true};

				auto closest_obst = *std::min_element(obst.begin(), obst.end(),
				                                          std::bind(sorting::sort_obst_by_distance, a, _1, _2));

				return correct_route(a, calc_avoid_point(a, b, closest_obst, correct_left), correct_left, max_corrections - 1);
			}

		public:
			FastPath(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore_list, double target_speed)
					: observer(observer), ignore_list(ignore_list), target_speed(target_speed) {}

			MovePromise navigate(const hlt::Ship *ship, const hlt::Vector &target) {
				ignore_list.push_back(ship->identify());
				auto correction_left = correct_route(ship->pos, target, true, 30);
				auto correction_right = correct_route(ship->pos, target, false, 30);

				hlt::Vector corrected_target = target;

				if (correction_left.second && correction_right.second) {
					const auto left_is_shorter = (ship->pos.dist(correction_left.first) + target.dist(correction_left.first) <
					                              ship->pos.dist(correction_right.first) + target.dist(correction_right.first));
					corrected_target =  left_is_shorter ? correction_left.first : correction_right.first;
				} else {
					if (correction_left.second) corrected_target = correction_left.first;
					if (correction_right.second) corrected_target = correction_right.first;
				}

				auto speed = std::min(ship->pos.dist(target), target_speed);
				auto vel = math::resize_line(ship->pos, corrected_target, speed) - ship->pos;
				return MovePromise::thrust(ship->entity_id, vel, target);
			}
		};
	}
}

#endif //HALITE_ORION_NAVIGATION_HPP_H
