#ifndef MUSICOS_ROLL_H
#define MUSICOS_ROLL_H

#include "musicos/command.h"
#include <algorithm>
#include <array>
#include <boost/regex.hpp>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <dpp/dispatcher.h>
#include <ios>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>
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
  bool has_calculation = false;
  bool is_error = false;
};

class roll : public command {
private:
  void error(const wiz::string &dice_string, const wiz::string &error_message,
             const int error_position = -1, const int error_position_length = 1);
  int apply_operator(int op1, int op2, char op, roll_flags_d flags, roll_result_d *result);

  int evaluate_expression(std::string expression, roll_flags_d flags);
  roll_flags_d parse_modifiers(std::string modifiers);

  std::string format_output(std::string input_string);

public:
  roll_d data;

  void parse_dice_string(const wiz::string &input);

  roll(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("roll", "#d#", bot_id)
                          .add_option(dpp::command_option(dpp::co_string, "dice", "#d#", true));
  }

  void command_definition() override;
};

#endif // MUSICOS_ROLL_H