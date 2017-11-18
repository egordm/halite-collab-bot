//
// Created by egordm on 16-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_ADVANCED_H
#define HALITE_ORION_NAVIGATION_ADVANCED_H

#include "../../hlt/vector.hpp"
#include "../../hlt/map.hpp"
#include "../../hlt/entities.hpp"
#include "../../hlt/move.hpp"
#include "../sorting.h"
#include "../math.hpp"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		static bool trajectories_colide(std::pair<hlt::Vector, hlt::Vector> t1, std::pair<hlt::Vector, hlt::Vector> t2) {
			// Find line functions
			auto tl1 = math::find_line(t1.first, t1.second);
			auto tl2 = math::find_line(t2.first, t2.second);
			auto intersection = math::intersection(tl1, tl2);

			// If trajectories dont intersect we check the closest distance
			//if (intersection.isnan()) {
			auto min_dist = std::min({t1.first.dist_line(t2.first, t2.second), t1.second.dist_line(t2.first, t2.second),
			                          t2.first.dist_line(t1.first, t1.second), t2.second.dist_line(t1.first, t1.second)});
			if(min_dist <= hlt::constants::SHIP_RADIUS * 2) return true;
			if (intersection.isnan()) return false;
			//}

			// We collide if distance from source to intersection is < sum of the radiuses
			auto dist1 = t1.first.dist(intersection);
			auto dist2 = t2.first.dist(intersection);
			return dist1 - dist2 <= hlt::constants::SHIP_RADIUS * 2 && dist1 <= hlt::constants::MAX_SPEED - hlt::constants::SHIP_RADIUS;
		}

		static hlt::Vector trajectory_collision_pos(std::pair<hlt::Vector, hlt::Vector> t1, std::pair<hlt::Vector, hlt::Vector> t2) {
			double max_dist = hlt::constants::SHIP_RADIUS * 2;

			if (t1.first.dist_line(t2.first, t2.second) <= max_dist) return t1.first.closest_point_line(t2.first, t2.second);
			if (t1.second.dist_line(t2.first, t2.second) <= max_dist) return t1.second.closest_point_line(t2.first, t2.second);
			if (t2.first.dist_line(t1.first, t1.second) <= max_dist) return t2.first.closest_point_line(t1.first, t1.second);
			if (t2.second.dist_line(t1.first, t1.second) <= max_dist) return t2.second.closest_point_line(t1.first, t1.second);


			auto tl1 = math::find_line(t1.first, t1.second);
			auto tl2 = math::find_line(t2.first, t2.second);
			auto intersection = math::intersection(tl1, tl2);
			return intersection;
		}

		constexpr double offset = 0.1;

		class AdvPath {
		private:
			hlt::Map *map;
			std::vector<hlt::EntityIdentifier> visited;
			bool first_section = true;
			unsigned long max_prop = 200;


			std::vector<std::shared_ptr<hlt::Entity>> check_obstacles(const hlt::Vector &a, const hlt::Vector &b, const bool check_vel) {
				std::vector<std::shared_ptr<hlt::Entity>> ret;
				for (const auto &kv : map->planets) {
					if (std::find(visited.begin(), visited.end(), kv.second->identify()) != visited.end()) continue;
					auto plane = kv.second.get();

					const auto dist_trajec = kv.second->pos.dist_line(a, b);
					if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) ret.push_back(kv.second);
				}
				for (const auto &kv : map->ships) {
					if (std::find(visited.begin(), visited.end(), kv.second->identify()) != visited.end()) continue;

					if (check_vel && kv.second->vel != hlt::Vector(0, 0)) {
						if (trajectories_colide({a, math::resize_line(a, b, hlt::constants::MAX_SPEED)},
						                        {kv.second->pos, kv.second->pos + kv.second->vel})) {
							ret.push_back(kv.second);
						}
					} else {
						const auto dist_trajec = kv.second->pos.dist_line(a, b);
						if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) ret.push_back(kv.second);
					}
				}
				return ret;
			}

			std::shared_ptr<hlt::Entity> closest_obstacle(const hlt::Vector &a, const hlt::Vector &b, const bool check_vel) {
				auto obstacles = check_obstacles(a, b, check_vel);
				if (obstacles.empty()) return nullptr;
				return *std::min_element(obstacles.begin(), obstacles.end(), std::bind(sorting::sort_by_distance, a, _1, _2));
			}

			/**
		 * Expand path in direction of first or second tan
		 * @param map
		 * @param a
		 * @param b point to expand
		 * @param c point to target tangents to if b is unreachable
		 * @param first_tan
		 * @return
		 */
			hlt::Vector expand_tangent(const hlt::Vector &a, const hlt::Vector &b, const hlt::Vector &c, bool first_tan,
			                           std::pair<hlt::Entity *, bool> &last) {
				auto obstacle = closest_obstacle(a, b, first_section);
				if (obstacle == nullptr) return b;
				auto test_obstacle = obstacle.get();

				if (last.first != nullptr && last.second != first_tan && obstacle->is_same(last.first)) return hlt::Vector::NAN_VEC;
				last = std::make_pair(obstacle.get(), first_tan);

				auto x = b.in_radius(obstacle->pos, obstacle->radius + hlt::constants::SHIP_RADIUS + offset) ? c : b;

				auto test_obst = obstacle.get();
				auto ex_tans = math::tangent_intersections(a, x, obstacle->pos, obstacle->radius + hlt::constants::SHIP_RADIUS + offset);

				const auto dist_trajec = obstacle->pos.dist_line(a, x);

				// Check if tangent intersection exists then return
				if (first_tan && !ex_tans.first.isnan()) {
					return expand_tangent(a, ex_tans.first, c, first_tan, last);
				}
				if (!first_tan && !ex_tans.second.isnan()) {
					return expand_tangent(a, ex_tans.second, c, first_tan, last);
				}

				// If there are no tangent interservsions
				return hlt::Vector::NAN_VEC;
			}

			/**
		 * Create a binary tree and pick the shortest path. Use intersections of tans as target points
		 * @param map
		 * @param a
		 * @param b
		 * @return
		 */
			std::pair<hlt::Vector, double> propagate_path(const hlt::Vector &a, const hlt::Vector &b, int depth,
			                                              std::pair<hlt::Entity *, bool> last) {
				//if(depth > 30) return {hlt::Vector::NAN_VEC, 99999};
				if (depth == 0) {
					auto t = true;
				}

				auto obstacle = closest_obstacle(a, b, first_section);
				if (obstacle == nullptr)
					return {b, a.dist(b)};

				auto test_obst = obstacle.get();
				auto obstacle_radius = obstacle->radius + hlt::constants::SHIP_RADIUS + offset;

				auto corrected_point = a;

				if (a.in_radius(obstacle->pos, obstacle_radius)) {
					corrected_point = a.closest_point(obstacle->pos, obstacle_radius + offset);
				}

				// Find intersections of tangents of a and b on c (extended tangents)
				auto tans = math::tangent_intersections(corrected_point, b, obstacle->pos, obstacle_radius);

				// TODO: go on and implement 4 way check expand one way and avoid mid two way
				// Expand to the side if a extended tangent is obstructed
				auto last1 = last;///
				auto last2 = last;
				auto tan_first1 = expand_tangent(a, tans.first, b, true, last1);
				auto tan_second1 = expand_tangent(a, tans.second, b, false, last2);
				if (last1.first == nullptr) last1 = {obstacle.get(), true};
				if (last2.first == nullptr) last2 = {obstacle.get(), false};


				// Moved to next section
				first_section = false;///

				std::pair<hlt::Vector, double> ret(hlt::Vector::NAN_VEC, 999999);

				// Find from which tangent the path to b is closer
				if (!tan_first1.isnan()) {
					if (depth == 0) {
						std::cout << "Path A" << tan_first1 << std::endl;
					}
					auto subpath1 = propagate_path(tan_first1, b, depth + 1, last1);
					if (!subpath1.first.isnan()) ret = {tan_first1, a.dist(tan_first1) + subpath1.second};
				}
				if (!tan_second1.isnan()) {
					if (depth == 0) {
						std::cout << "Path B" << tan_second1 << std::endl;
					}
					auto subpath2 = propagate_path(tan_second1, b, depth + 1, last2);
					auto dist2 = a.dist(tan_second1) + subpath2.second;
					if (!subpath2.first.isnan() && ret.second > dist2) ret = {tan_second1, dist2};
				}

				// Return shortest path and its length
				std::cout << "EndAdvPath" << depth << " at " << tan_first1 << std::endl;
				if (depth == 0) {
					std::cout << "End" << std::endl;
				}

				return ret;
			}

		public:
			AdvPath(hlt::Map *map, std::vector<hlt::EntityIdentifier> &visited) : map(map), visited(visited) {}

			hlt::Move navigate_towards(const hlt::Ship *ship, const hlt::Vector &target) {
				visited.push_back(ship->identify());

				// Find shortest path
				auto path = propagate_path(ship->pos, target, 0, {nullptr, false});

				const auto angle = static_cast<const int>(hlt::rad_to_deg(ship->pos.angle_between(path.first)));
				const auto speed = std::min(hlt::constants::MAX_SPEED, static_cast<const int &>(path.second));

				return hlt::Move::thrust(ship->entity_id, speed, angle);
			}
		};
	}
}

#endif //HALITE_ORION_NAVIGATION_ADVANCED_H
