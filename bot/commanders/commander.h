//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_COMMANDER_H
#define HALITE_ORION_COMMANDER_H


#include "observer.h"
#include "navigation/navigator.h"
#include "assignment.h"



namespace bot {
	namespace commanding { namespace behaviours {
		class BaseBehaviour;
	}}

	class Commander {
	protected:
		Observer &observer;
		navigation::Navigator *navigator;
		std::vector<commanding::behaviours::BaseBehaviour*> behaviours;

		hlt::entity_map<std::shared_ptr<bot::Assignment>> assignments;

		Commander(Observer &observer, navigation::Navigator *navigator) : observer(observer), navigator(navigator) {}

		virtual void clean();

		virtual void produce_assignments();

		virtual std::vector<hlt::Move> produce_moves();

	public:
		virtual ~Commander();

		virtual std::vector<hlt::Move> command();

		bool assign(const std::shared_ptr<bot::Assignment> &assignment);

		Observer &get_observer() const { return observer; }

		navigation::Navigator *get_navigator() const { return navigator; }

		const hlt::entity_map<std::shared_ptr<bot::Assignment>> &get_assignments() const { return assignments; }
	};
};


#endif //HALITE_ORION_COMMANDER_H
