//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_ASSIGNMENT_H
#define HALITE_ORION_ASSIGNMENT_H


#include <entities.hpp>
#include <memory>
#include <move.hpp>
#include "observer.h"
#include "navigator.h"

namespace bot {
	class Assignment {
	private:
		const std::shared_ptr<hlt::Ship> &ship;
	protected:
		explicit Assignment(const std::shared_ptr<hlt::Ship> &ship) : ship(ship) {}

	public:
		const std::shared_ptr<hlt::Ship> &get_ship() const { return ship; }

		virtual hlt::Move move(const Observer &observer, navigation::Navigator *navigator) const { return hlt::Move::noop(); };

		virtual bool is_valid(const Observer &observer) const { return get_ship()->is_alive(); }

		virtual unsigned int max_count(const Observer &observer) const { return 9001; }
	};

	template<class T>
	class TargetedAssignment : public Assignment {
	private:
		const std::shared_ptr<T> &target;
	protected:
		TargetedAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<T> &target) : Assignment(ship), target(target) {}

	public:
		virtual const std::shared_ptr<T> &get_target(const Observer &observer) const { return target; }

		bool is_valid(const Observer &observer) const override {
			return Assignment::is_valid(observer) && get_target(observer) != nullptr && get_target(observer)->is_alive();
		}
	};

	class ColonizeAssignment : public TargetedAssignment<hlt::Planet> {
	public:
		ColonizeAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &target)
				: TargetedAssignment(ship, target) {}

		hlt::Move move(const Observer &observer, navigation::Navigator *navigator) const override;

		bool is_valid(const Observer &observer) const override {
			return TargetedAssignment::is_valid(observer) && !get_target(observer)->is_full()
			       && !get_target(observer)->owner_mask(get_ship()->owner_id) != hlt::enemy_mask;
		}

		unsigned int max_count(const Observer &observer) const override { return get_target(observer)->spots_left(); }
	};

	class AttackShipAssignment : public TargetedAssignment<hlt::Ship> {
	public:
		AttackShipAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target)
				: TargetedAssignment(ship, target) {}

		hlt::Move move(const Observer &observer, navigation::Navigator *navigator) const override;

	};

	class AttackPlanetAssignment : public AttackShipAssignment {
	private:
		const std::shared_ptr<hlt::Planet> &target_planet;
	public:
		AttackPlanetAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &target)
				: AttackShipAssignment(ship, nullptr), target_planet(target) {}

		const std::shared_ptr<hlt::Planet> &get_target_planet() const { return target_planet; }

		const std::shared_ptr<hlt::Ship> &get_target(const Observer &observer) const override;

		bool is_valid(const Observer &observer) const override {
			return TargetedAssignment::is_valid(observer) && get_target_planet()->is_alive() &&
			       get_target_planet()->owner_mask(get_ship()->owner_id) == hlt::enemy_mask;
		}
	};
};


#endif //HALITE_ORION_ASSIGNMENT_H
