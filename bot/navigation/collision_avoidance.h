//
// Created by egordm on 19-11-2017.
//

#ifndef HALITE_ORION_COLLISION_AVOIDANCE_H
#define HALITE_ORION_COLLISION_AVOIDANCE_H


#include <types.hpp>
#include <observer.h>
#include <rvo/RVOSimulator.h>

namespace bot {
	namespace navigation {
		class NavigationCoordinator {
		private:
			Observer &observer;
			RVO::RVOSimulator *simulator;

			hlt::entity_map<unsigned long> ship_agent_lookup;
			hlt::entity_map<unsigned long> planet_agent_lookup;
			bool update_obstacles;

			template<typename T>
			void sync_entities(const std::vector<std::shared_ptr<T>> &entities, hlt::entity_map<unsigned long> &lookup,
			                   std::function<unsigned long(hlt::Entity *)> create_agent,  std::function<void(const hlt::EntityId)> remove_agent);

			unsigned long create_planet_agent(hlt::Entity *entity);

			unsigned long create_ship_agent(hlt::Entity *entity);

			void remove_ship_agent(hlt::EntityId entity_id);

			void remove_planet_agent(hlt::EntityId entity_id);

			void update_ship(hlt::Ship *ship);

		public:
			explicit NavigationCoordinator(Observer &observer);

			virtual ~NavigationCoordinator();

			void sync_observations();

			hlt::entity_map<hlt::Vector> calculate_velocities();

			static RVO::Vector2 convert(const hlt::Vector &v);

			static hlt::Vector convert(const RVO::Vector2 &v);
		};
	}
};


#endif //HALITE_ORION_COLLISION_AVOIDANCE_H
