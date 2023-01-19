#ifndef MUSICOS_COMMAND_H
#define MUSICOS_COMMAND_H

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
  // Optional function for doing initialization in derived commands.
  virtual void command_preprocess() {}
  // Optional function for executing the command.
  virtual void command_definition() {};

  // Reply override to add logging.
  void reply(dpp::command_completion_event_t callback = dpp::utility::log_error());
  // Reply override to add logging.
  void reply(dpp::interaction_response_type type, const dpp::message &message, const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());
  // Reply override to add logging.
  void reply(dpp::interaction_response_type type, const std::string &message, const std::string &log_message = "",
             dpp::command_completion_event_t callback = dpp::utility::log_error());
  // Reply override to add logging.
  void reply(const dpp::message &message, const std::string &log_message = "", dpp::command_completion_event_t callback = dpp::utility::log_error());
  // Reply override to add logging.
  void reply(const std::string &message, const std::string &log_message = "", dpp::command_completion_event_t callback = dpp::utility::log_error());

  // Edit response override to add logging.
  void edit_response(const dpp::message &message, dpp::command_completion_event_t callback = dpp::utility::log_error());
  // Edit response override to add logging.
  void edit_response(const std::string &message, dpp::command_completion_event_t callback = dpp::utility::log_error());

  // General purpose logging
  void log(const std::string &message);
  template<typename... Args>
  void log(fmt::format_string<Args...> fmt, Args &&...args);

public:
  dpp::slashcommand command_interface;

  virtual ~command() {}
  // Public interface for executing a command.
  void execute(dpp::slashcommand_t &e);
};

// Used in commands.cpp

extern std::list<command *> commands;
extern std::vector<dpp::slashcommand> commands_vector;
void initialize_commands(dpp::snowflake bot_id);

#endif // MUSICOS_COMMAND_H
