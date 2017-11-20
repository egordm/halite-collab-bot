//
// Created by egordm on 19-11-2017.
//


#include <algorithm>

#include <cmath>

#include <utils.h>
#include <constants.h>
#include <sstream>
#include "collision_avoidance.h"

// TODO: commit
// TODO: make it respect other ships
namespace bot {
	namespace navigation {

		NavigationCoordinator::NavigationCoordinator(Observer &observer) : observer(observer), simulator(new RVO::RVOSimulator()) {
		}

		NavigationCoordinator::~NavigationCoordinator() {
			delete simulator;
		}

		void NavigationCoordinator::sync_observations() {
			sync_entities(observer.get_planets(), planet_agent_lookup, std::bind(&NavigationCoordinator::create_planet_agent, this, _1),
			              std::bind(&NavigationCoordinator::remove_planet_agent, this, _1));
			const auto ships = observer.get_ships();
			sync_entities(ships, ship_agent_lookup, std::bind(&NavigationCoordinator::create_ship_agent, this, _1),
			              std::bind(&NavigationCoordinator::remove_ship_agent, this, _1));
			if(update_obstacles) {
				simulator->processObstacles();
				update_obstacles = false;
			}
			for (const auto &ship : ships) update_ship(ship.get());
		}

		hlt::entity_map<hlt::Vector> NavigationCoordinator::calculate_velocities() {
			simulator->doStep();
			hlt::entity_map<hlt::Vector> ret;
			for (const auto &kv : ship_agent_lookup) {
				ret.emplace(kv.first, convert(simulator->getAgentVelocity(kv.second)));
			}
			return ret;
		}

		void NavigationCoordinator::update_ship(hlt::Ship *ship) {
			if (ship_agent_lookup.find(ship->entity_id) == ship_agent_lookup.end() || ship->health <= 0) return;
			const auto agent_id = ship_agent_lookup.find(ship->entity_id)->second;

			simulator->setAgentPosition(agent_id, convert(ship->pos));
			simulator->setAgentVelocity(agent_id, convert(ship->vel));
			simulator->setAgentPrefVelocity(agent_id, convert(ship->vel));

			const bool do_sim = ship->docking_status == hlt::ShipDockingStatus::Undocked && ship->owner_id == observer.my_id;
			simulator->setAgentNeighborDist(agent_id, do_sim ? constants::NEIGHBOR_DIST : 0);
			simulator->setAgentMaxNeighbors(agent_id, do_sim ? constants::MAX_NEIGHBOURS : 0);
			simulator->setAgentTimeHorizon(agent_id, do_sim ? constants::TIME_HORIZON : 0);
			simulator->setAgentTimeHorizonObst(agent_id, do_sim ? constants::TIME_HORIZON_OBSTACLE : 0);
		}

		unsigned long NavigationCoordinator::create_ship_agent(hlt::Entity *entity) {
			const auto ret = simulator->addAgent(convert(entity->pos), 0, 0, 0, 0,
			                                     static_cast<float>(entity->radius) + constants::RADIUS_MARGIN,
			                                     hlt::constants::MAX_SPEED);
			update_ship(dynamic_cast<hlt::Ship *>(entity));
			return ret;
		}

		unsigned long NavigationCoordinator::create_planet_agent(hlt::Entity *entity) {
			std::vector<RVO::Vector2> vertices;
			const double angle_increment = M_PI * 2 / constants::PLANET_VERTICES;
			const double radius = entity->radius + constants::RADIUS_MARGIN;
			for (double angle = 0; angle <= 2 * M_PI; angle += angle_increment) {
				vertices.emplace_back(entity->pos.x + radius * cos(angle), entity->pos.y + radius * sin(angle));
			}
			update_obstacles = true;
			return simulator->addObstacle(vertices);
		}


		void NavigationCoordinator::remove_ship_agent(hlt::EntityId entity_id) {
			const auto &agent_id = ship_agent_lookup.at(entity_id);
			simulator->setAgentNeighborDist(agent_id, 0);
			simulator->setAgentMaxNeighbors(agent_id, 0);
			simulator->setAgentTimeHorizon(agent_id, 0);
			simulator->setAgentTimeHorizonObst(agent_id, 0);
			simulator->setAgentPosition(agent_id, RVO::Vector2(99999, 99999));
			simulator->setAgentPrefVelocity(agent_id, RVO::Vector2(0, 0));
			simulator->setAgentMaxSpeed(agent_id, 0);
			ship_agent_lookup.erase(ship_agent_lookup.find(entity_id));
		}

		void NavigationCoordinator::remove_planet_agent(hlt::EntityId entity_id) {
			const auto &agent_id = planet_agent_lookup.at(entity_id);
			planet_agent_lookup.erase(planet_agent_lookup.find(entity_id));
		}

		template<typename T>
		void NavigationCoordinator::sync_entities(const std::vector<std::shared_ptr<T>> &entities, hlt::entity_map<unsigned long> &lookup,
		                                          std::function<unsigned long(hlt::Entity *)> create_agent,
		                                          std::function<void(const hlt::EntityId)> remove_agent) {
			std::vector<hlt::EntityId> existing;
			for (const auto &entity : entities) {
				if (entity->health <= 0) continue;
				existing.push_back(entity->entity_id);
				if (lookup.find(entity->entity_id) != lookup.end()) continue;

				lookup.insert(std::make_pair(entity->entity_id, create_agent(entity.get())));
			}
			std::vector<hlt::EntityId> remove;
			for (const auto &kv : lookup) {
				if (std::find(existing.begin(), existing.end(), kv.first) == existing.end()) remove.push_back(kv.first);
			}
			for (const auto &rem : remove) remove_agent(rem);
		}

		RVO::Vector2 NavigationCoordinator::convert(const hlt::Vector &v) {
			return {static_cast<float>(v.x), static_cast<float>(v.y)};
		}

		hlt::Vector NavigationCoordinator::convert(const RVO::Vector2 &v) {
			return {v.x(), v.y()};
		}
	}
}