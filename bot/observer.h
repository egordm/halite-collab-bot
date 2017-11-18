//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include "../hlt/map.hpp"

namespace bot {
    class Observer {
    private:
        hlt::Map *map;
	    std::vector<std::shared_ptr<hlt::Ship>> ships;
	    std::vector<std::shared_ptr<hlt::Ship>> my_ships;
	    std::vector<std::shared_ptr<hlt::Ship>> enemy_ships;
	    std::vector<std::shared_ptr<hlt::Planet>> planets;
    public:
        const hlt::PlayerId my_id;
        unsigned int step;

        Observer(hlt::PlayerId id, hlt::Map *map);

	    virtual ~Observer();

	    void observe();

        const hlt::Map *getMap();

        std::vector<std::shared_ptr<hlt::Ship>> get_my_ships();

        std::vector<std::shared_ptr<hlt::Ship>> get_enemies();

        std::vector<std::shared_ptr<hlt::Ship>> get_ships();

	    std::vector<std::shared_ptr<hlt::Ship>> get_ships(const hlt::Vector &p, double radius, unsigned short owner_mask);

	    std::vector<std::shared_ptr<hlt::Ship>> get_ships(hlt::ShipDockingStatus status);

        std::shared_ptr<hlt::Ship> get_ship(hlt::EntityId ship_id);

	    std::vector<std::shared_ptr<hlt::Planet>> get_planets();

	    std::vector<std::shared_ptr<hlt::Planet>> get_planets(const hlt::Vector &p, double radius, unsigned short owner_mask);

	    std::vector<std::shared_ptr<hlt::Planet>> get_planets(unsigned short owner_mask);

	    std::shared_ptr<hlt::Planet> get_planet(hlt::EntityId planet_id);

	    std::vector<std::shared_ptr<hlt::Ship>> get_attackers(hlt::Planet * planet);

        std::vector<std::shared_ptr<hlt::Planet>> get_attacked_planets();

	    double get_production_share(hlt::PlayerId player_id);

	    double get_ship_share(hlt::PlayerId player_id);
    };
}

#endif //MYBOT_OBSERVER_H
