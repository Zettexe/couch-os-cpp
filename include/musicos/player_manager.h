#ifndef MUSICOS_PLAYER_MANAGER_H
#define MUSICOS_PLAYER_MANAGER_H

#include <dpp/discordclient.h>
#include <dpp/discordvoiceclient.h>
#include <dpp/dispatcher.h>
#include <dpp/snowflake.h>
#include <mutex>
#include <ogg/ogg.h>
#include <string>
#include <unordered_map>
#include <utility>

struct player_d {
  bool connected;
  dpp::discord_voice_client *voice_client;

  void stream(std::string file_path);
};

class player_manager_c {
private:
  static std::unordered_map<dpp::snowflake, player_d> players;
  std::mutex player_mutex;
  player_d *player;

  void assign_player(dpp::snowflake guild_id) {
    players.insert(std::make_pair(guild_id, player_d()));
    player = &players[guild_id];
  }

public:
  player_manager_c(){};
  ~player_manager_c(){};

  bool connect_voice(dpp::event_dispatch_t *event, dpp::guild guild, dpp::snowflake user_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild.id);

    dpp::voiceconn *voice_connection = event->from->get_voice(guild.id);

    // if (!player->voice_client) {
    //   player->voice_client = ->voiceclient;
    // }

    if (voice_connection) {
      auto user_voice_channel = guild.voice_members.find(user_id);
      player->connected = user_voice_channel != guild.voice_members.end() &&
                          voice_connection->channel_id == user_voice_channel->second.channel_id;

      if (player->connected) {
        return player->connected;
      }

      event->from->disconnect_voice(guild.id);
    }

    player->connected = guild.connect_member_voice(user_id, false, true);
    return player->connected;
  }

  void disconnect_voice(dpp::event_dispatch_t *event, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    event->from->disconnect_voice(guild_id);
    players.erase(players.find(guild_id));
  }

  void set_voice_client(dpp::discord_voice_client *voice_client, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->voice_client = voice_client;
  }

  bool player_is_connected(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return player->connected;
  }

  void stream_file(std::string file_path, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->stream(file_path);
  }
};

#endif // MUSICOS_PLAYER_MANAGER_H