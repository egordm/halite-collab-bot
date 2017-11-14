//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_ASSIGNMENT_H
#define HALITE_ORION_ASSIGNMENT_H


#include "../hlt/entities.hpp"
#include <memory>
#include "../hlt/move.hpp"
#include <sstream>
#include "../hlt/log.hpp"
#include "observer.h"
#include "navigator.h"

namespace bot {
	class Assignment {
	public:
		enum Type {
			ColonizePlanet = 0,
			DefendPlanet = 1,
			AttackPlanet = 2,
			DefendShip = 3,
			AttackShip = 4,
			NoOp = 5,
		};
	private:
		const std::shared_ptr<hlt::Ship> ship;
	protected:
		explicit Assignment(const std::shared_ptr<hlt::Ship> ship) : ship(ship) {}

	public:
		const std::shared_ptr<hlt::Ship> &get_ship() const { return ship; }

		virtual hlt::Move move(Observer &observer, navigation::Navigator *navigator) const { return hlt::Move::noop(); };

		virtual bool is_valid(Observer &observer) const { return get_ship()->is_alive(); }

		virtual unsigned int max_count(Observer &observer) const { return 9001; }

		virtual bool is_same(const std::shared_ptr<Assignment> &ass) { return get_type() == ass->get_type(); }

		virtual Type get_type() { return Type::NoOp; }

		virtual std::string str() {
			std::stringstream ss;
			ss << "Assigned ship: " << ship->entity_id << " Assignment Type: " << get_type();
			return ss.str();
		}
	};

	template<class T>
	class TargetedAssignment : public Assignment {
	public:
		const std::shared_ptr<T> target;
	protected:
		TargetedAssignment(const std::shared_ptr<hlt::Ship> &ship, std::shared_ptr<T> target) : Assignment(ship), target(target) {}

	public:
		virtual const std::shared_ptr<T> get_target(Observer &observer) const { return target; }

		bool is_valid(Observer &observer) const override {
			return Assignment::is_valid(observer) && get_target(observer) != nullptr && get_target(observer)->is_alive();
		}

		bool is_same(const std::shared_ptr<Assignment> &ass) override {
			return Assignment::is_same(ass) &&
			       target->entity_id == dynamic_cast<TargetedAssignment<T> *>(ass.get())->target->entity_id;
		}
	};

	class ColonizeAssignment : public TargetedAssignment<hlt::Planet> {
	public:
		ColonizeAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &target)
				: TargetedAssignment(ship, target) {}

		hlt::Move move(Observer &observer, navigation::Navigator *navigator) const override;

		bool is_valid(Observer &observer) const override {
			return TargetedAssignment::is_valid(observer) && !get_target(observer)->is_full()
			       && !get_target(observer)->owner_mask(get_ship()->owner_id) != hlt::enemy_mask && get_target(observer)->can_build_ships();
		}

		unsigned int max_count(Observer &observer) const override { return get_target(observer)->spots_left(); }

		Type get_type() override {
			return Type::ColonizePlanet;
		}
	};

	class AttackShipAssignment : public TargetedAssignment<hlt::Ship> {
	public:
		AttackShipAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target)
				: TargetedAssignment(ship, target) {}

		hlt::Move move(Observer &observer, navigation::Navigator *navigator) const override;

		unsigned int max_count(Observer &observer) const override { return 2; }

		Type get_type() override {
			return Type::AttackShip;
		}
	};

	class AttackPlanetAssignment : public AttackShipAssignment {
	private:
		const std::shared_ptr<hlt::Planet> target_planet;
	public:
		AttackPlanetAssignment(const std::shared_ptr<hlt::Ship> &ship, std::shared_ptr<hlt::Planet> target)
				: AttackShipAssignment(ship, nullptr), target_planet(std::move(target)) {}

		const std::shared_ptr<hlt::Planet> &get_target_planet() const { return target_planet; }

		const std::shared_ptr<hlt::Ship> get_target(Observer &observer) const override;

		bool is_valid(Observer &observer) const override {
			return TargetedAssignment::is_valid(observer) && get_target_planet()->is_alive() &&
			       get_target_planet()->owner_mask(get_ship()->owner_id) == hlt::enemy_mask;
		}

		unsigned int max_count(Observer &observer) const override;

		Type get_type() override {
			return Type::AttackPlanet;
		}

		bool is_same(const std::shared_ptr<Assignment> &ass) override {
			return Assignment::is_same(ass) &&
			       get_target_planet()->entity_id == dynamic_cast<AttackPlanetAssignment *>(ass.get())->get_target_planet()->entity_id;
		}
	};

	class DefendPlanetAssignment : public AttackPlanetAssignment {
	public:
		DefendPlanetAssignment(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &target)
				: AttackPlanetAssignment(ship, target) {}

		const std::shared_ptr<hlt::Ship> get_target(Observer &observer) const override;

		bool is_valid(Observer &observer) const override {
			return TargetedAssignment::is_valid(observer) && get_target_planet()->is_alive() &&
			       get_target_planet()->owner_mask(get_ship()->owner_id) == hlt::friendly_mask &&
					!observer.get_attackers(get_target_planet().get()).empty();
		}

		unsigned int max_count(Observer &observer) const override {
			return static_cast<unsigned int>(observer.get_attackers(get_target_planet().get()).size() + 1);
		}

		Type get_type() override {
			return Type::DefendPlanet;
		}
	};
};


#endif //HALITE_ORION_ASSIGNMENT_H
