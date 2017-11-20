//
// Created by egordm on 14-11-2017.
//

#ifndef HALITE_ORION_SMART_COMMANDER_H
#define HALITE_ORION_SMART_COMMANDER_H

#include <navigation/collision_avoidance.h>
#include "../commander.h"

namespace bot {
	namespace commanding {
		class SmarterCommander : public StrongerCommander {
		protected:
			std::shared_ptr<hlt::Planet> home_base = nullptr;
			navigation::NavigationCoordinator coordinator;

			void clean() override;

			void assign() override;

			void defend_pass();

			void discovery_pass();

			void assign(const std::shared_ptr<hlt::Ship> &ship) override;

			std::vector<hlt::Move> produce_moves() override;

		public:
			SmarterCommander(Observer &observer, navigation::Navigator *navigator)
					: StrongerCommander(observer, navigator), coordinator(navigation::NavigationCoordinator(observer)) {}
		};
	};
}


#endif //HALITE_ORION_SMART_COMMANDER_H
