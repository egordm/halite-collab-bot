//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_HPP_H
#define HALITE_ORION_NAVIGATION_HPP_H

#include <sstream>
#include <algorithm>
#include <math.hpp>
#include <functional>
#include <observer.h>
#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include "../utils.h"
#include "../sorting.h"
#include "collision_checking.hpp"
#include "ca_algorithms.hpp"

using namespace std::placeholders;

namespace bot { namespace navigation {
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
				case hlt::MoveType::Noop:return hlt::Move::noop();
				case hlt::MoveType::Thrust: {
					const double angle = std::round(hlt::rad_to_deg(velocity.angle()));
					const auto speed = std::min(7, static_cast<const int>(std::round(velocity.length())));
					return hlt::Move::thrust(ship_id, speed, static_cast<const int>(angle));
				}
				case hlt::MoveType::Dock:return hlt::Move::dock(ship_id, dock_to);
				case hlt::MoveType::Undock:return hlt::Move::undock(ship_id);
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

	constexpr double SHIP_TRAJ_CORRECT_RADIUS = 3 * hlt::constants::MAX_SPEED + 2 * hlt::constants::SHIP_RADIUS;
	constexpr double PLANET_TRAJ_CORRECT_RADIUS = hlt::constants::MAX_SPEED * 3;
	constexpr double VO_OFFSET = 0.1;

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
		virtual hlt::Vector pick_shortest_route(const hlt::Vector &a, const hlt::Vector &b, const std::vector<hlt::Vector> &aversions) {

			hlt::Vector ret = *std::min_element(aversions.begin(), aversions.end(),
			                                    [&a, &b](const hlt::Vector &e1, const hlt::Vector &e2) {
				                                    return !e1.isnan() && a.dist(e1) + b.dist(e1) < a.dist(e2) + b.dist(e2);
			                                    });

			return ret.isnan() ? hlt::Vector::NAN_VEC : ret;
		}

		virtual void get_obstacles(const hlt::Vector &a, const hlt::Vector &b, std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) {
			check_collisions(observer.get_planets(), a, b, ignore_list, obst);
			auto ships = observer.get_ships();
			utils::erase_if(ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
			check_collisions(ships, a, b, ignore_list, obst);
		}

		virtual hlt::Vector avoid_point(const hlt::Vector &a, const hlt::Vector &b, const std::pair<hlt::Entity *, hlt::Vector> &obstacle,
		                                const bool first_tangent) {
			if (a.in_radius(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS)) {
				return a.closest_point(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS);
			}

			auto tangents = a.tangents(obstacle.second, obstacle.first->radius + hlt::constants::SHIP_RADIUS);

			// Angle correction since converting it to int goes kind of bad
			return math::compensate_shitint(obstacle.second, a, first_tangent ? tangents.first : tangents.second);
		}

		virtual hlt::Vector correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_left, unsigned int max_corrections) {
			if (max_corrections == 0) return hlt::Vector::NAN_VEC;

			std::vector<std::pair<hlt::Entity *, hlt::Vector>> obst;
			get_obstacles(a, b, obst);
			if (obst.empty()) return b;

			const auto closest_obst = *std::min_element(obst.begin(), obst.end(), std::bind(sorting::sort_obst_by_distance, a, _1, _2));
			return correct_route(a, avoid_point(a, b, closest_obst, correct_left), correct_left, max_corrections - 1);
		}

		virtual hlt::Vector correct_velocity(const hlt::Vector &p, const hlt::Vector &v) {
			std::vector<std::tuple<hlt::Vector, hlt::Vector, double>> neighours;

			for (const auto &ship : observer.get_ships(p, SHIP_TRAJ_CORRECT_RADIUS, hlt::all_mask)) {
				if (std::find(ignore_list.begin(), ignore_list.end(), ship->identify()) != ignore_list.end()) continue;
				neighours.emplace_back(ship->pos, ship->vel, (ship->owner_mask(observer.my_id) == hlt::enemy_mask)
				                                             ? ship->radius * 2 : ship->radius + VO_OFFSET);
			}

			for (const auto &planet : observer.get_planets(p, PLANET_TRAJ_CORRECT_RADIUS, hlt::all_mask)) {
				if (std::find(ignore_list.begin(), ignore_list.end(), planet->identify()) != ignore_list.end()) continue;
				neighours.emplace_back(planet->pos, hlt::Vector(), planet->radius + VO_OFFSET);
			}

			const auto vos = bot::navigation::rvo::compute_velocity(p, hlt::constants::SHIP_RADIUS + VO_OFFSET, neighours);
			return bot::navigation::rvo::bruteforce(v, vos);
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
			if (corrected_target.isnan()) return MovePromise::noop();

			const auto ap_projection = math::resize_line(ship->pos, corrected_target, target_speed);
			std::vector<std::pair<hlt::Entity *, hlt::Vector>> proj_obst;
			get_obstacles(ship->pos, ap_projection, proj_obst);

			// Downtune overshooting
			auto speed = std::min(std::floor(ship->pos.dist(target)), target_speed);
			if (!proj_obst.empty()) {
				const auto closest_obst = *std::min_element(proj_obst.begin(), proj_obst.end(),
				                                            std::bind(sorting::sort_obst_by_distance, ship->pos, _1, _2));
				speed = std::floor(ship->pos.dist(ship->pos.closest_point(closest_obst.second, closest_obst.first->radius)));
			}

			auto vel = math::resize_line(ship->pos, corrected_target, speed) - ship->pos;
			vel = correct_velocity(ship->pos, vel);
			return MovePromise::thrust(ship->entity_id, vel, target);
		}
	};
}}

#endif //HALITE_ORION_NAVIGATION_HPP_H
