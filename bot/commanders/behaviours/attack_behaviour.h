//
// Created by egordm on 27-11-2017.
//

#ifndef HALITE_ORION_ATTACK_BEHAVIOUR_H
#define HALITE_ORION_ATTACK_BEHAVIOUR_H

#include "base_bahaviours.hpp"

namespace bot { namespace commanding { namespace behaviours {

	// TODO: split colonize, attack planet & attack ship
	class AttackBehaviour : public BaseBehaviour {
	public:
		explicit AttackBehaviour(Commander *commander) : BaseBehaviour(commander) {}

		/**
		 * Assign ship to a planet nearby
		 * @param ship
		 */
		void assign(const std::shared_ptr<hlt::Ship> &ship) const override {
			auto planets = commander->get_observer().get_planets(hlt::all_mask);
			while (!planets.empty()) {
				auto planet = utils::closest_object(planets, ship->pos);// TODO: calculate cost instead
				utils::erase(planets, planet.first);

				if ((hlt::empty_mask + hlt::friendly_mask) & planet.first->owner_mask(commander->get_observer().my_id)) {
					if (commander->assign(std::make_shared<ColonizeAssignment>(ship, planet.first))) return;
				} else {
					if (commander->assign(std::make_shared<AttackPlanetAssignment>(ship, planet.first))) return;
				}
			}

			// No good planet? Then attack a ship
			auto enemy = utils::closest_object(commander->get_observer().get_enemies(), ship->pos);
			if (enemy.second) commander->assign(std::make_shared<AttackShipAssignment>(ship, enemy.first));
		}

	};
}}}

#endif //HALITE_ORION_ATTACK_BEHAVIOUR_H
