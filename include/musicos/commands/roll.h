#ifndef MUSICOS_ROLL_H
#define MUSICOS_ROLL_H

#include "musicos/command.h"
#include <boost/regex.hpp>
#include <random>
#include <spdlog/spdlog.h>
#include <stack>
#include <wiz/string.h>

struct roll_flags_d {
  bool explode = false;
  int reroll_repeat = 1;
  std::pair<int, int> reroll;

  bool reverse_keep = false;
  int keep = 0;

  bool is_error = false;
};

struct roll_result_d {
  int total = 0;
  std::vector<int> result;
  std::vector<std::vector<int>> rolls;
  std::vector<std::pair<size_t, size_t>> reroll_positions;
};

struct roll_d {
  int total = 0;
  std::vector<roll_result_d> results = {};
  bool is_error = false;
};

class roll : public command {
private:
  void error(const wiz::string &dice_string, const wiz::string &error_message,
             const int error_position = -1, const int error_position_length = 1);
  int evaluate_expression(std::string expression, roll_flags_d flags);
  roll_flags_d parse_modifiers(std::string modifiers);

  void parse_dice_string(const wiz::string &input);

  std::string format_output(std::string input_string);

public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("roll", "#d#", event->command.application_id)
      .add_option(dpp::command_option(dpp::co_string, "dice", "#d#", true));
  }

  void command_definition() override;
};

#endif // MUSICOS_ROLL_H