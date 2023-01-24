#ifndef MUSICOS_COMMAND_H
#define MUSICOS_COMMAND_H

#include "musicos/player_manager.h"
#include <dpp/dispatcher.h>
#include <dpp/dpp.h>
#include <list>
#include <regex>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string_view>

class command {
private:
  const size_t message_size_limit = 2000;
  void log_reply(const std::string &message);
  void log_edit(const std::string &message);
  void log_generic(const std::string &message, const std::string &source);
  std::string to_string(const dpp::command_value &value);
  inline std::string limit_message_size(const std::string &message) {
    std::string limited_message;
    if (message.size() >= message_size_limit) {
      limited_message = message.substr(0, message_size_limit - 3) + "...";
    } else {
      limited_message = message;
    }

    return limited_message;
  }

protected:
  // Event associated with the current instance of a called command.
  dpp::slashcommand_t *event;
  // Event for use when calling from on_handle_message
  dpp::message_create_t *message_event;
  // Function for doing initialization in wrapper commands.
  virtual void command_preprocess() {}
  // Function for executing the command.
  virtual void command_definition() {}
  // Function for doing cleanup in wrapper commands.
  virtual void command_postprocess() {}

  // event.reply() with logging.
  void reply(dpp::command_completion_event_t callback = dpp::utility::log_error());
  // event.reply() with logging.
  void reply(dpp::interaction_response_type type, const dpp::message &message,
             const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());
  // event.reply() with logging.
  void reply(dpp::interaction_response_type type, const std::string &message,
             const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());
  // event.reply() with logging.
  void reply(dpp::message &message, const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());
  // event.reply() with logging.
  void reply(const std::string &message, const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());

  // event.edit_response() with logging.
  void edit_response(const dpp::message &message,
                     dpp::command_completion_event_t callback = dpp::utility::log_error());
  // event.edit_response() with logging.
  void edit_response(const std::string &message,
                     dpp::command_completion_event_t callback = dpp::utility::log_error());

  // General purpose logging
  void log(const std::string &message);
  // General purpose logging
  template <typename... Args> void log(fmt::format_string<Args...> fmt, Args &&...args);

public:
  dpp::slashcommand command_interface;

  virtual ~command() {}
  // Public interface for executing a command.
  void execute(dpp::slashcommand_t &e);
  void execute(dpp::message_create_t &e, std::string command_name);
};

// Used in commands.cpp

extern std::list<command *> commands;
extern std::vector<dpp::slashcommand> commands_vector;
void initialize_commands(dpp::snowflake bot_id, player_manager_c *player_manager);

#endif // MUSICOS_COMMAND_H
