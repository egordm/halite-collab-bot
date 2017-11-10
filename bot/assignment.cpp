//
// Created by egordm on 10-11-2017.
//

#include "assignment.h"

bot::Assignment::Assignment(hlt::EntityId ship, hlt::EntityId target, bot::Assignment::Type type) : ship_id(ship),
                                                                                                    target_id(target),
                                                                                                    type(type) {}
