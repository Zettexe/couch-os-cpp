#include "musicos/command.h"
#include "musicos/commands/roll.h"
#include "musicos/music_command.h"
#include "musicos/player_manager.h"
#include "wiz/string.h"
#include <chrono>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <dirent.h>
#include <dpp/appcommand.h>
#include <dpp/discordclient.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <exception>
#include <spdlog/common.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <thread>

int get_file_count() {
  int count = 0;

  // Open the folder
  DIR *dir = opendir("logs");
  if (dir == nullptr) {
    return 0;
  }

  // Format the current date in the same way as it appears in the file names
  std::time_t now = std::time(nullptr);
  std::tm now_tm = *std::localtime(&now);
  std::stringstream date_stream;
  date_stream << std::put_time(&now_tm, "%Y-%m-%d");
  wiz::string date_string = date_stream.str();

  // Iterate over the files in the folder
  dirent *file;
  while ((file = readdir(dir)) != nullptr) {
    // Check if the date appears in the file name
    if (wiz::string(file->d_name).find(date_string) != wiz::string::npos) {
      count++;
    }
  }

  // Close the folder
  closedir(dir);

  return count;
}

void initialize_logging() {
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());

  int count = get_file_count();
  if (count > 0) {
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_format_sink_st>(
      "logs/%Y-%m-%d." + std::to_string(count) + ".log", 0, 0));
  } else {
    sinks.push_back(
      std::make_shared<spdlog::sinks::daily_file_format_sink_st>("logs/%Y-%m-%d.log", 0, 0));
  }

  auto combined_logger = std::make_shared<spdlog::logger>("debug", begin(sinks), end(sinks));

  spdlog::register_logger(combined_logger);

  // Set the logger's log level
  combined_logger->set_level(spdlog::level::debug);
  combined_logger->flush_on(spdlog::level::debug);

  // Set the logger as the default logger
  spdlog::set_default_logger(combined_logger);
}

std::string fetch_egg() {
  curlpp::Easy request;

  // Set the request URL
  request.setOpt(new curlpp::options::Url(
    "https://api.unsplash.com/photos/"
    "random?query=egg&client_id=KiEb6iIfLHVH52tF2p6pbFvqNGCwT3NzdK4xoCa3nhg"));

  std::stringstream response;
  request.setOpt(new curlpp::options::WriteStream(&response));

  // Perform the request
  request.perform();

  nlohmann::json json_response = nlohmann::json::parse(response);

  return json_response["urls"]["raw"];
}

int main() {
  // Read the configuration file
  std::ifstream config_file("config.json");
  nlohmann::json config;
  config_file >> config;

  // Initialize the bot
  dpp::cluster bot(config["discord_token"], dpp::i_default_intents | dpp::i_message_content);
  player_manager_c player_manager = player_manager_c();

  initialize_logging();
  initialize_commands(bot.me.id, &player_manager);

  command::bot = &bot;

  bot.on_log([](const dpp::log_t &event) {
    switch (event.severity) {
      case dpp::ll_trace: spdlog::trace("{}", event.message); break;
      case dpp::ll_debug: spdlog::debug("{}", event.message); break;
      case dpp::ll_info: spdlog::info("{}", event.message); break;
      case dpp::ll_warning: spdlog::warn("{}", event.message); break;
      case dpp::ll_error: spdlog::error("{}", event.message); break;
      case dpp::ll_critical:
      default: spdlog::critical("{}", event.message); break;
    }
  });

  bot.on_slashcommand([](dpp::slashcommand_t event) {
    for (command *cmd : commands) {
      if (cmd->command_interface.name == event.command.get_command_name()) {
        cmd->execute(event);
        break;
      }
    }
  });

  bot.on_message_create([](const dpp::message_create_t &event) {
    if (event.msg.author.is_bot()) {
      return;
    }
    wiz::string message = event.msg.content;
    message = message.transform(::tolower);
    message = message.trim();

    if (message.starts_with("roll ")) {
      for (command *cmd : commands) {
        if (cmd->register_command().name == "roll") {
          cmd->execute((dpp::message_create_t &)event, "roll");
          break;
        }
      }
    } else if (message.contains("egg")) {
      event.reply(fetch_egg());
    } else if (message.starts_with("flip ")) {
      for (command *cmd : commands) {
        if (cmd->command_interface.name == "roll") {
          roll *roll_cmd = (roll *)cmd;
          if (message.starts_with("flip c")) {
            roll_cmd->parse_dice_string("1d2");
            event.reply(
              fmt::format("Flipped: **{}**", roll_cmd->data.total == 1 ? "Heads" : "Tails"));
          }
          break;
        }
      }
    }
  });

  bot.on_ready([&bot](dpp::ready_t) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_bulk_command_create({});
      // bot.global_bulk_command_create(commands_vector);
    }
  });

  bot.on_guild_create([&bot](dpp::guild_create_t event) {
    bot.guild_bulk_command_create(
      commands_vector, event.created->id, [event](dpp::confirmation_callback_t callback) {
        spdlog::info("GUILD CREATE: {}", event.created->name);

        if (callback.is_error()) {
          dpp::error_info error = callback.get_error();

          spdlog::error("({}): {}", error.code, error.message);

          for (const auto &error_detail : error.errors) {
            spdlog::error("DETAIL ({}): Field: {}, Object: {}, Reason: {}", error_detail.code,
                          error_detail.field, error_detail.object, error_detail.reason);
          }
          return;
        }

        std::stringstream command_names;
        for (const auto &[key, value] : std::get<dpp::slashcommand_map>(callback.value)) {
          if (value.options.empty()) {
            if (command_names.tellp() > 0) {
              command_names << ", ";
            }

            command_names << value.name;
            continue;
          }

          bool has_subcommands = false;

          for (size_t i = 0; i < value.options.size(); ++i) {
            const dpp::command_option &option = value.options[i];
            if (!(option.type == dpp::co_sub_command || option.type == dpp::co_sub_command_group)) {
              continue;
            }

            has_subcommands = true;

            if (option.type == dpp::co_sub_command) {
              if (command_names.tellp() > 0) {
                command_names << ", ";
              }
              command_names << value.name << " " << option.name;
              continue;
            }

            for (const dpp::command_option &option2 : option.options) {
              if (option2.type == dpp::co_sub_command) {
                if (command_names.tellp() > 0) {
                  command_names << ", ";
                }
                command_names << value.name << " " << option.name << " " << option2.name;
              }
            }
          }

          if (!has_subcommands) {
            if (command_names.tellp() > 0) {
              command_names << ", ";
            }

            command_names << value.name;
          }
        }
        spdlog::debug("Commands Created: {}", command_names.str());
      });
  });

  bot.on_voice_track_marker([&player_manager](dpp::voice_track_marker_t event) {
    std::vector<wiz::string> values = ((wiz::string)event.track_meta).split(":");

    if (values[0] == "end_of_stream") {
      player_manager.player_pop_queue(values[1]);
    }
  });

  bot.on_voice_ready([&player_manager](dpp::voice_ready_t event) {
    player_manager.set_voice_client(event.voice_client, event.voice_client->server_id);
  });

  bot.on_select_click([&bot, &player_manager](const dpp::select_click_t &event) {
    std::vector<wiz::string> values = ((wiz::string)event.values[0]).split(":");

    player_manager.download(values[0]);
    nlohmann::json video_info = player_manager.fetch_search_result(values[0]);
    player_manager.player_add_queue(video_info, event.command.guild_id);

    event.reply(dpp::ir_update_message, "Hahaha time to get deleted sucker");
    event.delete_original_response();
    bot.message_create(
      dpp::message(values[1], "").add_embed(player_manager.generate_embed(video_info)));
  });

  // bot.on_voice_state_update([](dpp::voice_state_update_t event){
  //   event.state
  // });

  // Run the bot
  bot.start(dpp::st_wait);
}
