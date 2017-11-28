//
// Created by egordm on 28-11-2017.
//

#ifndef HALITE_ORION_CENTRED_COMMANDER_H
#define HALITE_ORION_CENTRED_COMMANDER_H


#include <entities.hpp>
#include "commander.h"

namespace bot { namespace commanding {
	class CentredCommander : public Commander {
	protected:
		std::shared_ptr<hlt::Planet> home_base = nullptr;

		void clean() override;

		std::vector<hlt::Move> produce_moves() override;

	public:
		CentredCommander(Observer &observer, navigation::Navigator *navigator);

		const std::shared_ptr<hlt::Planet> &get_home_base() const {
			return home_base;
		}
	};
}}

#endif //HALITE_ORION_CENTRED_COMMANDER_H
