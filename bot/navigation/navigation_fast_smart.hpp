//
// Created by egordm on 18-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_FAST_SMART_HPP
#define HALITE_ORION_NAVIGATION_FAST_SMART_HPP


#include "navigation_advanced.hpp"
#include "collision_checking.hpp"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		constexpr double BYPASS_MARGIN = 0.2;

		class FastPath {
		private:
			Observer &observer;
			std::vector<hlt::EntityIdentifier> ignore;
			const unsigned int max_depth = 20;

			/**
			 * Planet obstacles
			 * @param a
			 * @param b
			 * @return
			 */
			std::vector<std::pair<hlt::Entity *, hlt::Vector>> check_planet_collisions(const hlt::Vector &a, const hlt::Vector &b) {
				auto planets = observer.get_planets();
				return navigation::check_collisions(planets, a, b, ignore);
			}

			std::vector<std::pair<hlt::Entity *, hlt::Vector>> check_ship_collisions(const hlt::Vector &a, const hlt::Vector &b) {
				std::vector<std::pair<hlt::Entity *, hlt::Vector>> ret;
				auto tp_shortened = math::resize_line(a, b, hlt::constants::MAX_SPEED);

				for (const auto &kv : observer.get_ships(tp_shortened, hlt::constants::MAX_SPEED * 2 + hlt::constants::SHIP_RADIUS * 2, hlt::all_mask)) {
					if (std::find(ignore.begin(), ignore.end(), kv->identify()) != ignore.end()) continue;

					// Check if ships trajectories collide
					std::pair<hlt::Vector, hlt::Vector> other_traj = {kv->pos, kv->pos + kv->vel};
					if (!trajectories_colide({a, tp_shortened}, other_traj)) continue;

					const auto collision_point = trajectory_collision_pos({a, tp_shortened}, other_traj);
					ret.emplace_back(kv.get(), collision_point);
				}
				return ret;
			};

			std::pair<hlt::Entity *, hlt::Vector> closest_obstacle(const hlt::Vector &a, const hlt::Vector &b) {
				std::vector<std::pair<hlt::Entity *, hlt::Vector>> obstacles;
				auto planet_obstacles = check_planet_collisions(a, b);
				auto ship_obstacles = check_ship_collisions(a, b);

				if(!planet_obstacles.empty()) obstacles.insert(obstacles.end(), planet_obstacles.begin(), planet_obstacles.end());
				if(!ship_obstacles.empty()) obstacles.insert(obstacles.end(), ship_obstacles.begin(), ship_obstacles.end());

				if (obstacles.empty()) return {nullptr, hlt::Vector::NAN_VEC};
				auto dist_sorter = [&a](const std::pair<hlt::Entity *, hlt::Vector> &e1,  const std::pair<hlt::Entity *, hlt::Vector> &e2) {
					return (e1.second.dist_sq(a)) < e2.second.dist_sq(a);
				};

				return *std::min_element(obstacles.begin(), obstacles.end(), dist_sorter);
			}

			hlt::Vector correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_first, unsigned int depth) {
				if (depth > max_depth) return hlt::Vector::NAN_VEC;

				auto obstacle = closest_obstacle(a, b);
				if (obstacle.first == nullptr) return b;

				auto tangents = a.tangents(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

				if(tangents.first.isnan()) tangents.first = a.closest_point(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);
				if(tangents.second.isnan()) tangents.second = a.closest_point(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

				if(a.dist(tangents.first) < hlt::constants::MAX_SPEED) tangents.first = math::resize_line(a, tangents.first, hlt::constants::MAX_SPEED);
				if(a.dist(tangents.second) < hlt::constants::MAX_SPEED) tangents.second = math::resize_line(a, tangents.second, hlt::constants::MAX_SPEED);
				return correct_route(a, (correct_first) ? tangents.first : tangents.second, correct_first, depth + 1);
			}

			hlt::Vector get_route(const hlt::Vector &a, const hlt::Vector &b) {
				auto correction_first = correct_route(a, b, true, 0);
				auto correction_second = correct_route(a, b, false, 0);

				if (correction_first.isnan() && correction_second.isnan()) return hlt::Vector::NAN_VEC;
				if (a.dist(correction_first) + b.dist(correction_first) < a.dist(correction_second) + b.dist(correction_second) &&
				    !correction_first.isnan()) {
					return correction_first;
				}
				return correction_second;
			}

		public:
			FastPath(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore) : observer(observer), ignore(ignore) {}

			hlt::Move navigate(const hlt::Ship *ship, const hlt::Vector &target) {
				ignore.push_back(ship->identify());

				auto target_point = get_route(ship->pos, target);


				if (target_point.isnan()) {
#if !NAVIGATION_BACKUP_LEGACY_MOVE
					return hlt::Move::noop();
#else
					const double angular_step_rad = M_PI / 180.0;
					return hlt::navigation::navigate_ship_towards_target(observer.getMap(), ship, target, hlt::constants::MAX_SPEED, true,
					                                                     hlt::constants::MAX_NAVIGATION_CORRECTIONS, angular_step_rad).first;
#endif
				}

				const double angle = std::round(hlt::rad_to_deg(ship->pos.angle_between(target_point)));
				const auto speed = std::min(7, std::max(0, static_cast<const int &>(std::round(ship->pos.dist(target)))));
				return hlt::Move::thrust(ship->entity_id, speed, static_cast<const int>(angle));
			}
		};

	}
}

#endif //HALITE_ORION_NAVIGATION_FAST_SMART_HPP
