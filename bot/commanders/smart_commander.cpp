//
// Created by egordm on 14-11-2017.
//

#include <algorithm>
#include <sorting.h>
#include <constants.h>
#include <utils.h>
#include <cmath>
#include "smart_commander.h"

using namespace std::placeholders;

namespace bot {
	namespace commanding {

		void SmarterCommander::clean() {
			Commander::clean();
			if (home_base != nullptr && (home_base->owner_mask(observer.my_id) != hlt::friendly_mask || !home_base->is_alive())) {
				home_base = nullptr;
			}
			// Pick the base planet
			// TODO: mb use average center
			if (home_base == nullptr) {
				const auto &my_planets = observer.get_planets(hlt::friendly_mask);
				if (!my_planets.empty()) {
					home_base = *std::max_element(my_planets.begin(), my_planets.end(), sorting::filter_by_size);
				}
			}
		}

		void SmarterCommander::assign() {
			defend_pass();
			Commander::assign();
		}

		void SmarterCommander::defend_pass() {
			auto attacked_planets = observer.get_attacked_planets();
			if (attacked_planets.empty()) return;
			std::sort(attacked_planets.begin(), attacked_planets.end(), std::bind(sorting::sort_by_distance, home_base->pos, _1, _2));

			auto undocked_ships = observer.get_my_ships();
			if (undocked_ships.empty()) return;
			utils::erase_if_not(undocked_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));

			// Do defend
			auto to_defend = static_cast<unsigned int>(std::ceil(undocked_ships.size() * constants::DEFEND_RATIO));
			for (const auto &planet : attacked_planets) {
				const auto max_defenders = std::max(1, static_cast<const int &>(observer.get_attackers(planet.get()).size() *
				                                                                constants::DEFENDERS_PER_ATTACKER));
				auto nearby_ships = observer.get_ships(planet->pos, planet->radius + constants::DEFEND_SEARCH_RADIUS, hlt::friendly_mask);
				if (nearby_ships.empty()) continue;

				utils::erase_if_not(nearby_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
				std::sort(nearby_ships.begin(), nearby_ships.end(), std::bind(sorting::sort_by_distance, planet->pos, _1, _2));

				for (unsigned int i = 0; i < nearby_ships.size() && i < max_defenders && to_defend > 0; ++i) {
					add_assignment(std::make_shared<DefendPlanetAssignment>(nearby_ships[i], planet));
					to_defend++;
				}
				if (to_defend <= 0) break;
			}

		}

		void SmarterCommander::assign(const std::shared_ptr<hlt::Ship> &ship) {
			StrongerCommander::assign(ship);
		}

		std::vector<hlt::Move> SmarterCommander::produce_moves() {
			std::vector<hlt::Move> ret;
			for (const auto &kv : assignments) {
				const auto &move = kv.second->move(observer, navigator);
				if(move.type == hlt::MoveType::Thrust) {
					kv.second->get_ship()->pos =
							kv.second->get_ship()->pos + hlt::Vector::from_angle(hlt::deg_to_rad(move.move_angle_deg), move.move_thrust);
				}
				ret.push_back(move);
			}

			return ret;
		}
	};
}