//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include "../hlt/map.hpp"

namespace bot {
    class Observer {
    private:
        const hlt::Map &map;
	    std::vector<std::shared_ptr<hlt::Ship>> ships;
	    std::vector<std::shared_ptr<hlt::Ship>> my_ships;
	    std::vector<std::shared_ptr<hlt::Ship>> enemy_ships;
	    std::vector<std::shared_ptr<hlt::Planet>> planets;
    public:
        const hlt::PlayerId my_id;
        unsigned int step;

        Observer(hlt::PlayerId id, const hlt::Map &map);

        void observe();

        const hlt::Map &getMap() const;

        std::vector<std::shared_ptr<hlt::Ship>> &get_my_ships() const;

        std::vector<std::shared_ptr<hlt::Ship>> &get_enemies() const;

        std::vector<std::shared_ptr<hlt::Ship>> &get_ships() const;

	    std::vector<std::shared_ptr<hlt::Ship>> get_ships(const hlt::Vector &p, double radius, unsigned short owner_mask) const;

        std::shared_ptr<hlt::Ship> get_ship(hlt::EntityId ship_id);

	    std::vector<std::shared_ptr<hlt::Planet>> &get_planets();

	    std::vector<std::shared_ptr<hlt::Planet>> get_planets(unsigned short owner_mask) const;

	    std::shared_ptr<hlt::Planet> get_planet(hlt::EntityId planet_id);

	    std::vector<std::shared_ptr<hlt::Ship>> get_attackers(hlt::Planet * planet) const;

        std::vector<std::shared_ptr<hlt::Planet>> get_attacked_planets() const;
    };
}

#endif //MYBOT_OBSERVER_H
