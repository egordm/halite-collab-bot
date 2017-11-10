//
// Created by Cody on 10/11/2017.
//

#ifndef MYBOT_COMMANDER_H
#define MYBOT_COMMANDER_H

#include <vector>
#include <entities.hpp>
#include "hlt/map.hpp"

namespace bot {
    enum SIDE {
        OUR, ENEMY, NEUTRAl
    };
    enum ACTION {
        GROUP, DOCK, ATTACK
    };

    struct BotMove {
        hlt::Entity target;
        hlt::Vector pos;
        SIDE side;
        ACTION action;
    };

    class Commander {
    public:
        std::vector<BotMove> GetMoves(hlt::Map& map);
    };
}
#endif //MYBOT_COMMANDER_H