#ifndef MUSICOS_PLAYER_MANAGER_H
#define MUSICOS_PLAYER_MANAGER_H

#include "wiz/cache.h"
#include <chrono>
#include <cstdlib>
#include <deque>
#include <dpp/colors.h>
#include <dpp/discordclient.h>
#include <dpp/discordvoiceclient.h>
#include <dpp/dispatcher.h>
#include <dpp/snowflake.h>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <mutex>
#include <ogg/ogg.h>
#include <string>
#include <unordered_map>
#include <utility>

struct player_d {
  bool connected;
  dpp::discord_voice_client *voice_client;
  std::deque<nlohmann::json> queue;

  void stream(std::string file_path);
};

class player_manager_c {
private:
  wiz::cache<nlohmann::json> search_cache = wiz::cache<nlohmann::json>(std::chrono::seconds(300));
  std::unordered_map<dpp::snowflake, player_d> players;
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

  void player_add_queue(nlohmann::json video_data, dpp::snowflake guild_id,
                        bool add_front = false) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    if (add_front) {
      player->queue.push_front(video_data);
    } else {
      player->queue.push_back(video_data);
    }
  }

  void player_pop_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->queue.pop_front();
    if (player->queue.size() > 0) {
      player->stream(player->queue[0]["id"]);
    }
  }

  void stream_file(std::string video_id, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->stream(video_id);
  }

  dpp::embed generate_embed(nlohmann::json video_json) {
    int seconds = video_json["duration"].get<int>();
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    std::string duration = fmt::format("{}:{}", minutes, seconds);
    if (hours > 0) {
      duration = fmt::format("{}:{}", hours, duration);
    }

    dpp::embed embed = dpp::embed()
                         .set_color(dpp::colors::scarlet_red)
                         .set_title(video_json["title"].get<std::string>())
                         .set_url(video_json["webpage_url"].get<std::string>())
                         .set_description(" ")
                         .set_thumbnail(video_json["thumbnail"].get<std::string>())
                         .add_field("", "Added To Queue", true)
                         .add_field("", duration, true);
    //  .add_field("Next up:", "[title](url)");
    return embed;
  }

  nlohmann::json fetch_search_result(std::string input) {
    auto [result, value] = search_cache.get(input);

    if (result) {
      return value;
    }

    std::string command =
      "yt-dlp --dump-json --flat-playlist --compat-options no-youtube-unavailable-videos \"" +
      input + "\" 2>/dev/null";
    std::string output;

    FILE *pipe = popen(command.c_str(), "r");
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
      output += buffer;
    }
    pclose(pipe);

    size_t last_newline_pos = output.find_last_of("\n");
    size_t pos = output.find("\n");
    while (pos != last_newline_pos) {
      output.replace(pos, 1, ",");
      pos = output.find("\n");
    }

    output = "[" + output + "]";

    nlohmann::json json_output = nlohmann::json::parse(output);

    if (json_output.size() == 0) {
      return json_output;
    }

    nlohmann::json filtered_output;

    std::vector<std::string> fields_to_copy = {"id", "title", "webpage_url", "duration",
                                               "uploader"};

    for (auto &obj : json_output) {
      nlohmann::json new_obj;
      for (auto &[key, value] : obj.items()) {
        if (std::find(fields_to_copy.begin(), fields_to_copy.end(), key) != fields_to_copy.end()) {
          new_obj[key] = value;
        }
      }

      for (auto thumbnail : obj["thumbnails"]) {
        if (thumbnail.find("height") != thumbnail.end()) {
          new_obj["thumbnail"] = thumbnail["url"];
          break;
        }
      }

      if (new_obj.find("thumbnail") == new_obj.end()) {
        new_obj["thumbnail"] = obj["thumbnails"][0]["url"];
      }

      filtered_output.push_back(new_obj);
      search_cache.put(new_obj["id"].get<std::string>(), new_obj);
    }

    return filtered_output;
  }

  void download(std::string video_id) {
    if (std::filesystem::exists(fmt::format("{}.opus", video_id))) {
      return;
    }

    std::string t = fmt::format("yt-dlp -f 251 --http-chunk-size 2M '{}' -x "
                                "--audio-format opus --audio-quality 0 -o 'music/{}.ogg'",
                                video_id, video_id);

    system(t.c_str());
  }
};

#endif // MUSICOS_PLAYER_MANAGER_H