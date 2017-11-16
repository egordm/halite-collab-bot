//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_COMMANDER_H
#define HALITE_ORION_COMMANDER_H


#include "observer.h"
#include "navigation/navigator.h"
#include "assignment.h"

namespace bot {
	class Commander {
	protected:
		Observer &observer;
		navigation::Navigator *navigator;

		hlt::entity_map<std::shared_ptr<bot::Assignment>> assignments;

		Commander(Observer &observer, navigation::Navigator *navigator) : observer(observer), navigator(navigator) {}

		virtual void clean();

		virtual void assign();

		virtual std::vector<hlt::Move> produce_moves();

		virtual void assign(const std::shared_ptr<hlt::Ship> &ship) {}

	public:

		virtual ~Commander();

		virtual std::vector<hlt::Move> command();


		bool add_assignment(const std::shared_ptr<bot::Assignment> &assignment);
	};

	class JuniorCommander : public Commander {
	protected:
		void assign(const std::shared_ptr<hlt::Ship> &ship) override;

	public:
		JuniorCommander(Observer &observer, navigation::Navigator *navigator) : Commander(observer, navigator) {}

	};

	class StrongerCommander : public Commander {
	protected:
		void assign(const std::shared_ptr<hlt::Ship> &ship) override;

	public:
		StrongerCommander(Observer &observer, navigation::Navigator *navigator) : Commander(observer, navigator) {}
	};
};


#endif //HALITE_ORION_COMMANDER_H
