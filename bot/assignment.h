//
// Created by egordm on 10-11-2017.
//

#ifndef MYBOT_ASSIGNMENT_H
#define MYBOT_ASSIGNMENT_H


#include <sstream>
#include "../hlt/move.hpp"
#include "../hlt/types.hpp"
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

        hlt::EntityId ship_id;

        explicit Assignment(hlt::EntityId ship_id) : ship_id(ship_id) {}

        virtual ~Assignment() = default;

        virtual hlt::nullable<hlt::Move> get_move(Observer &observer, Navigator *navigator) {
            return std::make_pair(hlt::Move::noop(), false);
        }

        virtual bool is_valid(Observer &observer) {
            return false;
        }

        hlt::nullable<hlt::Ship> get_ship(Observer &observer);

        virtual unsigned int max_assignments(Observer &observer) {
            return 9001;
        }

        virtual Assignment::Type get_type() const {
            return Assignment::Type::NoOp;
        }

        bool operator==(const Assignment &rhs) const;

        bool operator!=(const Assignment &rhs) const;

        virtual std::string str() {
            std::stringstream ss;
            ss << "Assigned ship: " << ship_id << " Assignment Type: " << get_type();
            return ss.str();
        }
    };

    template<class T>
    class TargetedAssignment : public Assignment {
    public:
        hlt::EntityId target_id;

        TargetedAssignment(hlt::EntityId ship_id, hlt::EntityId target_id)
                : Assignment(ship_id), target_id(target_id) {}

        hlt::nullable<hlt::Move> get_move(Observer &observer, Navigator *navigator) override {
            return std::make_pair(hlt::Move::noop(), (get_ship(observer).second && get_target(observer).second));
        }

        virtual hlt::nullable<T> get_target(Observer &observer) {
            return std::make_pair(T(), false);
        }

        bool is_valid(Observer &observer) override {
            const auto &ship = get_ship(observer);
            const auto &target = get_target(observer);
            return (ship.second && ship.first.is_alive() && get_target(observer).second && target.first.is_alive());
        }

        bool operator==(const TargetedAssignment &rhs) const {
            return static_cast<const Assignment &>(*this) == static_cast<const Assignment &>(rhs) &&
                   target_id == rhs.target_id;
        }

        bool operator!=(const TargetedAssignment &rhs) const {
            return !(rhs == *this);
        }

        std::string str() override {
            std::stringstream ss;
            ss << "Assigned ship: " << ship_id << " Target " << target_id << " Assignment Type: " << get_type();
            return ss.str();
        }
    };

    class ColonizeAssignment : public TargetedAssignment<hlt::Planet> {
    public:
        ColonizeAssignment(hlt::EntityId ship_id, hlt::EntityId target_id)
                : TargetedAssignment(ship_id, target_id) {}

        hlt::nullable<hlt::Planet> get_target(Observer &observer) override;

        hlt::nullable<hlt::Move> get_move(Observer &observer, Navigator *navigator) override;

        bool is_valid(Observer &observer) override;

        unsigned int max_assignments(Observer &observer) override;

        Assignment::Type get_type() const override {
            return Assignment::Type::ColonizePlanet;
        }
    };

    class AttackShipAssignment : public TargetedAssignment<hlt::Ship> {
    public:
        AttackShipAssignment(hlt::EntityId ship_id, hlt::EntityId target_id) : TargetedAssignment(ship_id, target_id) {}

        hlt::nullable<hlt::Move> get_move(Observer &observer, Navigator *navigator) override;

        hlt::nullable<hlt::Ship> get_target(Observer &observer) override;

        Assignment::Type get_type() const override {
            return Assignment::Type::AttackShip;
        }
    };

    class AttackPlanetAssignment : public AttackShipAssignment {
    public:
        AttackPlanetAssignment(hlt::EntityId ship_id, hlt::EntityId target_id)
                : AttackShipAssignment(ship_id, target_id) {}

        hlt::nullable<hlt::Ship> get_target(Observer &observer) override;


        Assignment::Type get_type() const override {
            return Assignment::Type::AttackPlanet;
        }
    };

}


#endif //MYBOT_ASSIGNMENT_H
