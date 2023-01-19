#ifndef MUSICOS_MUSIC_COMMAND_H
#define MUSICOS_MUSIC_COMMAND_H

#include "musicos/command.h"
#include <mutex>

struct player_d {};

class music_command : public command {
private:
  static std::unordered_map<dpp::snowflake, player_d> players;

protected:
  static std::mutex player_mutex;
  player_d *player;

  std::lock_guard<std::mutex> lock() {
    std::lock_guard<std::mutex> player_lock(player_mutex);
    return player_lock;
  }

public:
  void command_preprocess() override {
    lock();
    // std::lock_guard<std::mutex> lock(player_mutex);
    players.insert({event->command.guild_id, player_d()});
    player = &players[event->command.guild_id];
  }
};

#endif // MUSICOS_MUSIC_COMMAND_H