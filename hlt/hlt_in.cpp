#include "hlt_in.hpp"
#include "log.hpp"
#include "hlt_out.hpp"

namespace hlt {
	namespace in {
		static std::string g_bot_name;
		static int g_map_width;
		static int g_map_height;
		static int g_turn = 0;

		void setup(const std::string &bot_name, int map_width, int map_height) {
			g_bot_name = bot_name;
			g_map_width = map_width;
			g_map_height = map_height;
		}

		void update_map(const Map &map) {
			if (g_turn == 1) {
				out::send_string(g_bot_name);
			}

			const std::string input = get_string();

			if (!std::cin.good()) {
				// This is needed on Windows to detect that game engine is done.
				std::exit(0);
			}

			if (g_turn == 0) {
				Log::log("--- PRE-GAME ---");
			} else {
				Log::log("--- TURN " + std::to_string(g_turn) + " ---");
			}
			++g_turn;

			update_map(input, map);
		}

		static std::shared_ptr<Ship> parse_ship(std::stringstream &iss, const PlayerId owner_id, const Map &map) {
			EntityId entity_id;
			iss >> entity_id;

			bool new_entity = (map.ships.find(entity_id) == map.ships.end());
			auto ship = (!new_entity) ? map.ships.at(entity_id) : std::make_shared<Ship>();
			ship->entity_id = entity_id;

			Vector pos;
			iss >> pos.x;
			iss >> pos.y;
			if (!new_entity) ship->velocity = pos - ship->pos;
			ship->pos = pos;

			iss >> ship->health;

			// No longer in the game, but still part of protocol.
			double vel_x_deprecated, vel_y_deprecated;
			iss >> vel_x_deprecated;
			iss >> vel_y_deprecated;

			int docking_status;
			iss >> docking_status;
			ship->docking_status = static_cast<ShipDockingStatus>(docking_status);

			iss >> ship->docked_planet;
			iss >> ship->docking_progress;
			iss >> ship->weapon_cooldown;

			ship->owner_id = owner_id;
			ship->radius = constants::SHIP_RADIUS;

			return ship;
		}

		static std::shared_ptr<Planet> parse_planet(std::istream &iss, const Map &map) {
			EntityId entity_id;
			iss >> entity_id;
			auto planet = (map.planets.find(entity_id) != map.planets.end())
			              ? map.planets.at(entity_id) : std::make_shared<Planet>();
			planet->entity_id = entity_id;

			iss >> planet->pos.x;
			iss >> planet->pos.y;
			iss >> planet->health;
			iss >> planet->radius;
			iss >> planet->max_docking_spots;
			iss >> planet->current_production;
			iss >> planet->remaining_production;

			int owned;
			iss >> owned;
			if (owned == 1) {
				planet->owned = true;
				int owner;
				iss >> owner;
				planet->owner_id = static_cast<PlayerId>(owner);
			} else {
				planet->owned = false;
				int false_owner;
				iss >> false_owner;
				planet->owner_id = -1;
			}

			unsigned int num_docked_ships;
			iss >> num_docked_ships;

			planet->docked_ships.clear();
			planet->docked_ships.reserve(num_docked_ships);
			for (unsigned int i = 0; i < num_docked_ships; ++i) {
				EntityId ship_id;
				iss >> ship_id;
				planet->docked_ships.push_back(ship_id);
			}

			return planet;
		}

		static void update_map(const std::string &input, const Map &map) {
			std::stringstream iss(input);

			int num_players;
			iss >> num_players;

			// TODO: this is just a workaround for missing ships. Mayby there isa betetr approach
			for (auto kv : map.ships) {
				kv.second->health = 0;
			}
			for (auto kv : map.planets) {
				kv.second->health = 0;
			}
			entity_map<std::shared_ptr<Ship>> ships;
			entity_map<std::shared_ptr<Planet>> planets;

			// Parse players
			for (int i = 0; i < num_players; ++i) {
				PlayerId player_id;
				int player_id_int;
				iss >> player_id_int;
				player_id = static_cast<PlayerId>(player_id_int);

				unsigned int num_ships;
				iss >> num_ships;

				// Parse ships for player
				std::vector<std::shared_ptr<Ship>> player;
				for (unsigned int j = 0; j < num_ships; ++j) {
					const auto &ship = parse_ship(iss, player_id, map);
					ships.insert(std::make_pair(ship->entity_id, ship));
					player.push_back(ship);
				}
				map.players[player_id] = player;
			}
			map.ships = ships;

			// Parse planets
			unsigned int num_planets;
			iss >> num_planets;

			planets.reserve(num_planets);
			for (unsigned int i = 0; i < num_planets; ++i) {
				const auto &planet = parse_planet(iss, map);
				planets.insert(std::make_pair(planet->entity_id, planet));
			}
			map.planets = planets;
		}
	}
}
