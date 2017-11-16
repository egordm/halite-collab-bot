//
// Created by egordm on 15-11-2017.
//

#include <map.hpp>
#include <hlt_in.hpp>
//#include <navigation.hpp>
#include "navigation/navigation_smart.hpp"

std::string env = " 2 0 2 2 125.1865 64.3791 255 0.0000 0.0000 2 3 0 0 7 121.9251 78.0749 255 0.0000 0.0000 0 0 0 0 1 2 4 114.8135 95.6209 255 0.0000 0.0000 2 1 0 0 6 118.0749 81.9251 255 0.0000 0.0000 0 0 0 0 12 0 127.1431 87.1431 1160 4.5509 2 0 655 0 0 0 1 112.8569 87.1431 1160 4.5509 2 0 655 1 1 1 4 2 112.8569 72.8569 1160 4.5509 2 0 655 0 0 0 3 127.1431 72.8569 1160 4.5509 2 0 655 1 0 1 2 4 74.1732 129.0574 1616 6.3394 3 0 912 0 0 0 5 42.5409 50.9764 1616 6.3394 3 0 912 0 0 0 6 165.8268 30.9426 1616 6.3394 3 0 912 0 0 0 7 197.4591 109.0236 1616 6.3394 3 0 912 0 0 0 8 222.2183 63.0575 1958 7.6795 3 0 1105 0 0 0 9 148.0610 141.7167 1958 7.6795 3 0 1105 0 0 0 10 17.7817 96.9425 1958 7.6795 3 0 1105 0 0 0 11 91.9390 18.2833 1958 7.6795 3 0 1105 0 0 0";

std::shared_ptr<hlt::Map> make_env() {
	auto map = std::make_shared<hlt::Map>(300, 300);

	auto ship = std::make_shared<hlt::Ship>();
	ship->pos = hlt::Vector(10, 10);

	map->ships.insert(std::make_pair(ship->entity_id, ship));

	auto p1 = std::make_shared<hlt::Planet>();
	p1->entity_id = 0;
	p1->pos = hlt::Vector(30, 50);
	p1->radius = 20;

	auto p2 = std::make_shared<hlt::Planet>();
	p1->entity_id = 1;
	p1->pos = hlt::Vector(50, 70);
	p1->radius = 12;

	auto p3 = std::make_shared<hlt::Planet>();
	p1->entity_id = 2;
	p1->pos = hlt::Vector(70, 46);
	p1->radius = 12;

	auto p4 = std::make_shared<hlt::Planet>();
	p1->entity_id = 3;
	p1->pos = hlt::Vector(41, 27);
	p1->radius = 18;

	map->planets.insert(std::make_pair(p1->entity_id, p1));
	map->planets.insert(std::make_pair(p2->entity_id, p2));
	map->planets.insert(std::make_pair(p3->entity_id, p3));
	map->planets.insert(std::make_pair(p4->entity_id, p4));

	return map;
}

void test_path_legacy() {
	//auto map = make_env();
	auto *map = new hlt::Map(240, 160);
	hlt::in::update_map(env, map);
	auto ship = map->ships.at(7);
	auto target = map->ships.at(6);
	//hlt::Vector target(80, 100);


	unsigned int step = 0;
	std::vector<hlt::Vector> step_pos;
	auto test = ship.get();
	auto dd = target.get();
	while (ship->pos.dist(target->pos) > 7) {
		std::vector<hlt::EntityId> visited;
		//auto move = hlt::navigation::navigate_ship_towards_target(map.get(), ship.get(), target, 7, true, 180, 1);

		visited.push_back(ship->entity_id);
		visited.push_back(target->entity_id);

		auto move = bot::navigation::navigate_towards(map, ship.get(), target->pos, visited);
		auto delta = hlt::Vector::from_angle(hlt::deg_to_rad(move.first.move_angle_deg), move.first.move_thrust);
		ship->pos = ship->pos + delta;
		if (move.second) std::cout << "Move by: " << delta << " Angle: " << move.first.move_angle_deg % 360 << std::endl;
		step_pos.push_back(ship->pos);
		step++;
	}
	for (const auto &pos : step_pos) {
		std::cout << "," << pos;
	}
	std::cout << std::endl << "Finished in " << step << std::endl;

	//delete map;
}

int main() {
	test_path_legacy();
	//std::cout << hlt::Vector(67.12, -37.8).dist_line(hlt::Vector(31.5, -76.21), hlt::Vector(147.11, -57.7)) << std::endl;
}