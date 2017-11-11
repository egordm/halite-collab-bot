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

        hlt::entity_map<bot::Assignment*> assignments;
    public:
        Commander(Observer &observer, Navigator *navigator);

        virtual ~Commander();

        std::vector<hlt::Move> command();

        bot::Assignment* assign(const hlt::Ship &ship);

        void add_assignment(bot::Assignment* assignment);

        bool can_assign(bot::Assignment* assignment);
    };
}


#endif //MYBOT_COMMANDER_H
