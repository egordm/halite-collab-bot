
#ifndef MYBOT_ENTITIES_H
#define MYBOT_ENTITIES_H

#include <vector>
#include "vector.hpp"
#include "types.hpp"
#include "log.hpp"

namespace hlt {
	const unsigned short empty_mask = 1;
	const unsigned short friendly_mask = 2;
	const unsigned short enemy_mask = 4;
	const unsigned short all_mask = enemy_mask + friendly_mask + empty_mask;

	enum class EntityType {
		None = 0,
		Ship = 1,
		Planet = 2
	};

	using EntityIdentifier = std::pair<EntityId, EntityType>;

	struct Entity {
		Entity() = default;

		EntityId entity_id{};
		PlayerId owner_id{};
		Vector pos{};
		int health{};
		double radius{};

		bool is_alive() const {
			return health > 0;
		}

		virtual unsigned short owner_mask(const PlayerId &player_id) const {
			return owner_id == player_id ? hlt::friendly_mask : hlt::enemy_mask;
		}

		virtual EntityType get_type() const { return EntityType::None; }

		virtual EntityIdentifier identify() const { return {entity_id, get_type()};}

		bool is_same(const Entity * entity) {
			return entity->get_type() == get_type() && entity->entity_id == entity_id;
		}
	};

	enum class ShipDockingStatus {
		Undocked = 0,
		Docking = 1,
		Docked = 2,
		Undocking = 3,
	};

	struct Planet : public Entity {
		bool owned;

		/// The remaining resources.
		int remaining_production;

		/// The currently expended resources.
		int current_production;

		/// The maximum number of ships that may be docked.
		unsigned int max_docking_spots;

		/// Contains IDs of all ships in the process of docking or undocking,
		/// as well as docked ships.
		std::vector<EntityId> docked_ships;

		unsigned int spots_left() { return static_cast<unsigned int>(max_docking_spots - docked_ships.size()); }

		bool is_full() const {
			return docked_ships.size() == max_docking_spots;
		}

		unsigned short owner_mask(const PlayerId &player_id) const override {
			if (!owned) return hlt::empty_mask;
			return Entity::owner_mask(player_id);
		}


		bool can_build_ships() {
			return (constants::RESOURCES_FOR_SHIP - current_production % constants::RESOURCES_FOR_SHIP) <= remaining_production;
		}

		EntityType get_type() const override {
			return EntityType::Planet;
		}
	};

	struct Ship : public Entity {
		/// The turns left before the ship can fire again.
		int weapon_cooldown{};

		ShipDockingStatus docking_status;

		Vector vel{};

		/// The number of turns left to complete (un)docking.
		int docking_progress{};

		EntityId docked_planet{};

		bool can_dock(const Planet *planet) const {
			return pos.dist(planet->pos) <= (constants::SHIP_RADIUS + constants::DOCK_RADIUS + planet->radius);
		}

		EntityType get_type() const override {
			return EntityType::Ship;
		}
	};


}


#endif //MYBOT_ENTITIES_H
