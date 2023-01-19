#ifndef MUSICOS_MUSIC_COMMAND_H
#define MUSICOS_MUSIC_COMMAND_H

#include "musicos/command.h"
#include <mutex>
#include <string>

struct player_d {};

class music_command : public command {
  // private:
  //   static std::unordered_map<dpp::snowflake, player_d> players;

protected:
  // static std::mutex player_mutex;
  // player_d *player;

  bool join_voice() {
    dpp::guild guild = event->command.get_guild();
    dpp::voiceconn *voice_connection = event->from->get_voice(event->command.guild_id);
    if (voice_connection) {
    }
    if (guild.connect_member_voice(event->command.usr.id, false, true)) {
      log(std::to_string(voice_connection->channel_id));
    }
    return voice_connection;
  }

  // void command_preprocess() override {
  //   // std::lock_guard<std::mutex> lock(player_mutex);
  //   // players.insert({event->command.guild_id, player_d()});
  //   // player = &players[event->command.guild_id];
  //   // dpp::voiceconn *test = event->from->get_voice(event->command.guild_id);
  //   // test->voiceclient->is_ready();
  // }
};

#endif // MUSICOS_MUSIC_COMMAND_H