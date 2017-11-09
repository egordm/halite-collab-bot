//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include <map.hpp>

namespace bot {
    class Observer {
    private:
        hlt::Map map;
        std::unordered_map<unsigned int, hlt::Vector> velocities;
    public:
        const hlt::PlayerId id;
        unsigned int step;

        Observer(hlt::PlayerId id, const hlt::Map &map);

        void observe(const hlt::Map &new_map);

        const hlt::Map &getMap() const;
    };
}

#endif //MYBOT_OBSERVER_H
