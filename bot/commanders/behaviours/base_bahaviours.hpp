//
// Created by egordm on 27-11-2017.
//

#ifndef HALITE_ORION_BASE_BAHAVIOURS_HPP
#define HALITE_ORION_BASE_BAHAVIOURS_HPP


#include <commanders/commander.h>
#include <constants.h>
#include <commanders/centred_commander.h>

namespace bot { namespace commanding { namespace behaviours {
	class BaseBehaviour {
	protected:
		Commander *commander;
	public:
		explicit BaseBehaviour(Commander *commander) : commander(commander) {}

		virtual void assign() const {
			for (const auto &ship : commander->get_observer().get_my_ships()) {
				if (commander->get_assignments().find(ship->entity_id) != commander->get_assignments().end()) continue;
				if (ship->docking_status != hlt::ShipDockingStatus::Undocked) continue;
				assign(ship);
			}
		}

		virtual void assign(const std::shared_ptr<hlt::Ship> &ship) const {};
	};

	class CentredBehaviour : public BaseBehaviour {
	public:
		explicit CentredBehaviour(CentredCommander *commander) : BaseBehaviour(commander) {}

		hlt::Vector get_home_base() const {
			return dynamic_cast<CentredCommander *>(commander)->get_home_base()->pos;
		}
	};
}}}

#endif //HALITE_ORION_BASE_BAHAVIOURS_HPP
