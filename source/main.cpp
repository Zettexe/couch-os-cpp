#include "musicos/command.h"
#include "musicos/commands/roll.h"
#include "musicos/music_command.h"
#include "musicos/player_manager.h"
#include "wiz/string.h"
#include <cctype>
#include <chrono>
#include <cstdio>
#include <dirent.h>
#include <dpp/dispatcher.h>
#include <dpp/dpp.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sstream>

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

  // Use daily_file_sink to create a new file every day
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
  combined_logger->set_level(spdlog::level::info);
  combined_logger->flush_on(spdlog::level::info);

  // Set the logger as the default logger
  spdlog::set_default_logger(combined_logger);
}

int main() {
  initialize_logging();

  // Read the configuration file
  std::ifstream config_file("config.json");
  nlohmann::json config;
  config_file >> config;

  // Initialize the bot
  dpp::cluster bot(config["discord_token"], dpp::i_default_intents | dpp::i_message_content);
  player_manager_c player_manager = player_manager_c();

  initialize_commands(bot.me.id, &player_manager);

  bot.on_slashcommand([](dpp::slashcommand_t event) {
    // Iterate over the list of commands
    for (command *cmd : commands) {
      // Check if the command name matches the command invoked in the event
      if (cmd->command_interface.name == event.command.get_command_name()) {
        // Call the command function of the matching Command object
        cmd->execute(event);
        break; // Exit the loop once the command has been called
      }
    }
  });

  bot.on_message_create([](const dpp::message_create_t &event) {
    wiz::string message = event.msg.content;
    message = message.transform(::tolower);
    if (message.starts_with("roll")) {
      for (command *cmd : commands) {
        if (cmd->command_interface.name == "roll") {
          cmd->execute((dpp::message_create_t &)event, "roll");
          break;
        }
      }
    }
  });

  // bot.on_ready([&bot](dpp::ready_t) {
  //   if (dpp::run_once<struct register_bot_commands>()) {
  //     bot.global_bulk_command_create(commands_vector);
  //   }
  // });

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
        spdlog::info("Commands Created: {}", command_names.str());
      });
  });

  bot.on_voice_ready([&player_manager](dpp::voice_ready_t event) {
    player_manager.set_voice_client(event.voice_client, event.voice_client->server_id);
    player_manager.stream_file("music/test_song.opus", event.voice_client->server_id);
  });

  // bot.on_voice_state_update([](dpp::voice_state_update_t event){
  //   event.state
  // });

  // Run the bot
  bot.start(dpp::st_wait);
}
