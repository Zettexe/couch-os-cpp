#ifndef MUSICOS_PLAYER_MANAGER_H
#define MUSICOS_PLAYER_MANAGER_H

#include "wiz/cache.h"
#include <algorithm>
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
#include <future>
#include <memory>
#include <mutex>
#include <ogg/ogg.h>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

struct player_queue {
  nlohmann::json video_data;
  std::shared_ptr<std::future<void>> download_progress;
};

struct player_d {
  bool connected;
  bool paused;
  bool data_loaded;
  int looping = 0;
  dpp::discord_voice_client *voice_client;
  std::deque<player_queue> queue;

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

  bool player_is_data_loaded(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return player->data_loaded;
  }

  void player_clear_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->queue.clear();
    player->voice_client->stop_audio();
  }

  void player_pop_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    if (player->queue.size() > 0) {
      player->queue.pop_front();
      if (player->queue.size() > 0) {
        player->queue[0].download_progress->wait();
        player->stream(player->queue[0].video_data["id"]);
      }
    }
  }

  void player_start_stream(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    if (player->queue.size() > 0) {
      if (player->queue[0].download_progress && player->queue[0].download_progress->valid()) {
        player->queue[0].download_progress->wait();
      }
      player->stream(player->queue[0].video_data["id"]);
    }
  }

  int player_get_looping(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return player->looping;
  }

  void player_set_looping(int times, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->looping = times;
  }

  void player_skip(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->voice_client->skip_to_next_marker();
  }

  void player_pause(bool pause, dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->paused = pause;
    player->voice_client->pause_audio(pause);
  }

  void player_unset_data_loaded(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->data_loaded = false;
  }

  bool player_paused(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return player->paused;
  }

  bool player_playing(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return !player->voice_client->is_paused();
  }

  std::deque<player_queue> *player_get_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    return &player->queue;
  }

  std::string get_formatted_timestamp(int seconds) {
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    std::stringstream timestamp;
    if (hours > 0) {
      timestamp << hours << ":";
    }
    timestamp << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

    return timestamp.str();
  }

  dpp::embed generate_single_embed(nlohmann::json video_json, std::string title) {
    std::string duration = get_formatted_timestamp(video_json["duration"].get<int>());

    return dpp::embed()
      .set_color(dpp::colors::scarlet_red)
      .set_title(title)
      .set_description(fmt::format("[{}]({})", video_json["title"].get<std::string>(),
                                   video_json["webpage_url"].get<std::string>()))
      .set_thumbnail(video_json["thumbnail"].get<std::string>())
      .add_field("", video_json["uploader"].get<std::string>(), true)
      .add_field("", duration, true);
    ;
  }

  dpp::embed generate_multi_embed(nlohmann::json video_json, std::string title) {
    std::string description;

    for (size_t i = 0; i < std::min<size_t>(5, video_json.size()); i++) {
      nlohmann::json item = video_json[i];
      if (description != "") {
        description += "\n";
      }
      description +=
        fmt::format("[{}] [{}]({})", get_formatted_timestamp(item["duration"].get<int>()),
                    item["title"].get<std::string>(), item["webpage_url"].get<std::string>());
    }

    if (video_json.size() > 5) {
      description += fmt::format("\n+{} more...", video_json.size() - 5);
    }

    return dpp::embed()
      .set_color(dpp::colors::scarlet_red)
      .set_title(title)
      .set_description(description);
  }

  dpp::embed generate_embed(nlohmann::json video_json, std::string title) {
    if (video_json.size() > 1) {
      return generate_multi_embed(video_json, title);
    }
    return generate_single_embed(video_json[0], title);
  }

  std::tuple<bool, dpp::embed> generate_queue_embed(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);

    std::string now_playing_str = "";
    std::string queue_str = "";

    for (size_t i = 0; i < std::min<size_t>(5, player->queue.size()); i++) {
      player_queue item = std::move(player->queue.at(i));

      if (now_playing_str == "") {
        now_playing_str =
          fmt::format("[{}] [{}]({})", get_formatted_timestamp(item.video_data["duration"]),
                      item.video_data["title"], item.video_data["webpage_url"]);
        continue;
      }

      queue_str +=
        fmt::format("[{}] [{}]({})\n", get_formatted_timestamp(item.video_data["duration"]),
                    item.video_data["title"], item.video_data["webpage_url"]);
    }

    if (player->queue.size() > 5) {
      queue_str += fmt::format("+{} more...", player->queue.size() - 5);
    }

    dpp::embed embed = dpp::embed()
                         .set_color(dpp::colors::scarlet_red)
                         .set_title("Queue")
                         .add_field("Now Playing:", now_playing_str);

    if (!queue_str.empty()) {
      embed.add_field("Coming next:", queue_str);
    }

    return std::make_tuple(now_playing_str == "", embed);
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

  std::future<void> download(std::string video_id) {
    if (std::filesystem::exists(fmt::format("music/{}.opus", video_id))) {
      // TODO: Check if file is corrupt
      return std::async([]() {});
    }

    return std::async([&video_id]() {
      std::string t = fmt::format("yt-dlp -f 251 --http-chunk-size 2M '{}' -x "
                                  "--audio-format opus --audio-quality 0 -o 'music/{}.ogg'",
                                  video_id, video_id);
      system(t.c_str());
    });
  }

  void download_and_add_to_queue(nlohmann::json video_data, dpp::snowflake guild_id) {
    player_queue queue_item = player_queue();
    queue_item.video_data = video_data;
    queue_item.download_progress =
      std::make_shared<std::future<void>>(download(video_data["id"].get<std::string>()));

    std::lock_guard<std::mutex> lock(player_mutex);
    assign_player(guild_id);
    player->queue.push_back(queue_item);

    if (player->voice_client) {
      if (!player->data_loaded) {
        player->stream(player->queue[0].video_data["id"]);
        return;
      }

      if (player->paused) {
        player->paused = false;
        player->voice_client->pause_audio(false);
      }
    }
  }
};

#endif // MUSICOS_PLAYER_MANAGER_H