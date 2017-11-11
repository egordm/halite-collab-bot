//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_COMMANDER_H
#define MYBOT_COMMANDER_H

#include "navigator.h"
#include "observer.h"
#include "assignment.h"

namespace bot {
    class Commander {
    private:
        Observer &observer;
        Navigator *navigator;

        std::vector<bot::Assignment> assignments;
    public:
        Commander(Observer &observer, Navigator *navigator);

        std::vector<hlt::Move> command();

        bot::Assignment assign(const hlt::Ship &ship);

        hlt::Move produce_move(const bot::Assignment &assignment);

        hlt::nullable<hlt::Ship> attack_planet(const hlt::Ship &ship, const hlt::Planet &planet);

        bool valid_assignment(bot::Assignment ass);

        hlt::nullable<bot::Assignment> get_assignment(const hlt::EntityId &ship_id);
    };
}


#endif //MYBOT_COMMANDER_H
