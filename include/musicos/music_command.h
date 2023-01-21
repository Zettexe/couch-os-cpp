#ifndef MUSICOS_MUSIC_COMMAND_H
#define MUSICOS_MUSIC_COMMAND_H

#include "musicos/command.h"
#include <dpp/discordclient.h>

struct player_d {
  bool connected;
  dpp::voiceconn *voice_connection;
};

class music_command : public command {
private:
  static std::unordered_map<dpp::snowflake, player_d> players;
  std::mutex player_mutex;

protected:
  player_d *player;

  void join_voice() {
    dpp::guild guild = event->command.get_guild();

    if (!player->voice_connection) {
      player->voice_connection = event->from->get_voice(guild.id);
    }

    if (player->voice_connection) {
      auto user_voice_channel = guild.voice_members.find(event->command.usr.id);
      player->connected =
        user_voice_channel != guild.voice_members.end() &&
        player->voice_connection->channel_id == user_voice_channel->second.channel_id;

      if (player->connected) {
        return;
      }

      event->from->disconnect_voice(guild.id);
    }

    player->connected = guild.connect_member_voice(event->command.usr.id, false, true);
    return;
  }

  void command_preprocess() override {
    player_mutex.lock();
    dpp::guild guild = event->command.get_guild();

    players.insert(std::make_pair(guild.id, player_d()));
    player = &players[guild.id];
  }

  void command_postprocess() override { player_mutex.unlock(); }
};

#endif // MUSICOS_MUSIC_COMMAND_H