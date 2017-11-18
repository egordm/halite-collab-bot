//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_SMART_HPP_H
#define HALITE_ORION_NAVIGATION_SMART_HPP_H

#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include <algorithm>
#include "../sorting.h"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		namespace smart {
			constexpr double BYPASS_MARGIN = 0.1;

			static hlt::Vector extend_tangent(const hlt::Vector &a, const hlt::Vector &b, const hlt::Vector &t) {

			}

			static std::pair<bool, hlt::Vector> correct_route(const hlt::Map *map, const hlt::Vector &a, const hlt::Vector &b,
			                                                  std::vector<hlt::EntityId> visited) {
				// Find planets intersecting path
				std::vector<std::shared_ptr<hlt::Entity>> obst;
				for (const auto &kv : map->planets) {
					const auto dist_trajec = kv.second->pos.dist_line(a, b);
					if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) obst.push_back(kv.second);
				}
				for (const auto &kv : map->ships) {
					const auto dist_trajec = kv.second->pos.dist_line(a, b);
					if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) obst.push_back(kv.second);
				}
				if (obst.empty()) return std::make_pair(true, b);

				// Check if we already visited this obstacle. This eliminates inf loops
				auto closest_obstacle = *std::min_element(obst.begin(), obst.end(), std::bind(sorting::sort_by_distance, a, _1, _2));
				for (const auto &vis : visited) {
					if (vis == closest_obstacle->entity_id) return std::make_pair(false, b);
				}
				visited.push_back(closest_obstacle->entity_id);

				// Pick check if either of tangents leads to the right path
				// TODO: mb a 90deg angle between point and centre on the tangent
				auto test = closest_obstacle->pos;
				auto test2 = closest_obstacle->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN;
				auto tangents = a.tangents(closest_obstacle->pos, closest_obstacle->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);
				if (a.dist(tangents.second) + b.dist(tangents.second) < a.dist(tangents.first) + b.dist(tangents.first)) {
					tangents = std::make_pair(tangents.second, tangents.first);
				}
				auto first_tan_target = correct_route(map, a, tangents.first, visited);
				if (first_tan_target.first && correct_route(map, tangents.first, b, visited).first) {
					return first_tan_target;
				}
				auto second_tan_target = correct_route(map, a, tangents.second, visited);
				if (second_tan_target.first && correct_route(map, tangents.second, b, visited).first) {
					return second_tan_target;
				}

				return std::make_pair(false, b);
			}


			static hlt::nullable<hlt::Move>
			navigate_towards(const hlt::Map *map, hlt::Ship *ship, const hlt::Vector &target, std::vector<hlt::EntityId> &visited) {
				auto corrected_target = correct_route(map, ship->pos, target, visited);
				if (!corrected_target.first) corrected_target.second = target;

				const double angle = ship->pos.angle_between(corrected_target.second);

				return {hlt::Move::thrust(ship->entity_id, std::min(7, std::max(1, static_cast<const int &>(ship->pos.dist(target)))),
				                          static_cast<const int>(hlt::rad_to_deg(angle))), true};
			}

			static hlt::nullable<hlt::Move> navigate_ship_to_dock(
					const hlt::Map *map,
					hlt::Ship *ship,
					hlt::Entity *dock_target) {
				const hlt::Vector &target = ship->pos.closest_point(dock_target->pos, dock_target->radius + hlt::constants::DOCK_RADIUS);

				std::vector<hlt::EntityId> test;
				test.push_back(ship->entity_id);
				test.push_back(dock_target->entity_id);
				return navigate_towards(map, ship, target, test);
			}
		}
	}
}

#endif //HALITE_ORION_NAVIGATION_SMART_HPP_H
