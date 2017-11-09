//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include <map.hpp>

class Observer {
private:
    hlt::PlayerId id;
    hlt::Map map;
    unsigned int step;

    std::unordered_map<unsigned int, hlt::Vector> velocities;
public:
    Observer(hlt::PlayerId id, const hlt::Map &map);

    void observe(const hlt::Map &new_map);
};


#endif //MYBOT_OBSERVER_H
