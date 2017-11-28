//
// Created by egordm on 13-11-2017.
//

#include <algorithm>
#include "log.hpp"
#include "commander.h"
#include "behaviours/base_bahaviours.hpp"


bot::Commander::~Commander() {
	delete navigator;
	for(auto &behaviour : behaviours) delete behaviour;
	behaviours.clear();
}

std::vector<hlt::Move> bot::Commander::command() {
	clean();
	produce_assignments();
	return produce_moves();
}

void bot::Commander::clean() {
	assignments.clear();
	utils::erase_if(assignments, [this](std::pair<hlt::EntityId, std::shared_ptr<Assignment>> ass) {
		return !ass.second->is_valid(observer);
	});
}

void bot::Commander::produce_assignments() {
	for(const auto &behaviour : behaviours) behaviour->assign();
}

bool bot::Commander::assign(const std::shared_ptr<bot::Assignment> &assignment) {
	if (assignment != nullptr && !assignment->is_valid(observer)) return false; // Als if exceeds assignment count

	unsigned int assigned_count = 0;
	for (const auto &ass : assignments) if (assignment->is_same(ass.second)) assigned_count++;
	if (assigned_count >= assignment->max_count(observer)) return false;

	assignments.insert(std::make_pair(assignment->get_ship()->entity_id, assignment));
	return true;
}

std::vector<hlt::Move> bot::Commander::produce_moves() {
	std::vector<hlt::Move> ret;
	for (const auto &kv : assignments) {
#if DEBUG
		hlt::Log::log(kv.second->str());
#endif
		kv.second->produce_move(navigator);
	}

	return navigator->produce_moves();
}