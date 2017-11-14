#include <iostream>
#include "hlt/hlt.hpp"
#include "bot/defines.h"
#include "bot/bot.h"

int main() {
    const hlt::Metadata metadata = hlt::initialize(BOT_NAME);
    const hlt::PlayerId player_id = metadata.player_id;
	hlt::Map *initial_map = metadata.initial_map;

	bot::Bot bot(player_id, initial_map);

	while(true) {
		if (!hlt::out::send_moves(bot.do_step())) {
			hlt::Log::log("send_moves failed; exiting");
			break;
		}
	}
    return 0;
}