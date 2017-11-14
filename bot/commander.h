//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_COMMANDER_H
#define HALITE_ORION_COMMANDER_H


#include "observer.h"
#include "navigator.h"
#include "assignment.h"

namespace bot {
	class Commander {
	protected:
		Observer &observer;
		navigation::Navigator *navigator;

		hlt::entity_map<std::shared_ptr<bot::Assignment>> assignments;

		Commander(Observer &observer, navigation::Navigator *navigator) : observer(observer), navigator(navigator) {}

	public:

		virtual ~Commander();

		virtual std::vector<hlt::Move> command();

		virtual void assign(const std::shared_ptr<hlt::Ship> &ship) { }

		bool add_assignment(const std::shared_ptr<bot::Assignment> &assignment);
	};

	class JuniorCommander : public Commander {
	public:
		JuniorCommander(Observer &observer, navigation::Navigator *navigator) : Commander(observer, navigator) {}

		void assign(const std::shared_ptr<hlt::Ship> &ship) override;
	};

	class StrongerCommander : public Commander {
	public:
		StrongerCommander(Observer &observer, navigation::Navigator *navigator) : Commander(observer, navigator) {}

		void assign(const std::shared_ptr<hlt::Ship> &ship) override;
	};
};


#endif //HALITE_ORION_COMMANDER_H
