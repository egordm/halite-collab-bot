//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_NAVIGATOR_H
#define MYBOT_NAVIGATOR_H

#include <move.hpp>
#include "observer.h"


namespace bot {
    struct NavEnv {
        // TODO: mb move max corrections
        std::vector<hlt::Planet> planets;
        std::vector<hlt::Ship> ships;
        int max_corrections = 5;

        NavEnv();

        NavEnv(std::vector<hlt::Planet> planets, std::vector<hlt::Ship> ships);

        NavEnv(Observer &observer, const hlt::Ship &ship);

        template<class T>
        hlt::Move avoid(const hlt::Ship &ship, const T &target, int max_correctons);

        hlt::Move move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles);

        hlt::Move move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles, int max_correctons);

        void remove(const hlt::Ship &ship);
        void remove(const hlt::Planet &planet);
    };

    class Navigator {
    private:
        Observer &observer;
    public:
        explicit Navigator(Observer &observer);

        hlt::Move dock_planet(const hlt::Ship &ship, const hlt::Planet &planet);

        hlt::Move attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel);
    };

    template<class T>
    hlt::nullable<T> obj_between(const std::vector<T> &objs, const hlt::Vector &a, const hlt::Vector &b) {
        for (const auto &obj : objs) {
            if (obj.pos.dist_line(a, b) <= obj.radius + hlt::constants::SHIP_RADIUS) {
                return std::make_pair(obj, true);
            }
        }
        return std::make_pair(T(), false);
    }
}

#endif //MYBOT_NAVIGATOR_H
