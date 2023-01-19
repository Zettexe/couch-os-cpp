#include "musicos/commands/roll.h"

roll_d data;

void roll::error(const wiz::string &dice_string, const wiz::string &error_message,
                 const int error_position, const int error_position_length) {
  std::string error_line = "";
  for (int i = 0; i < static_cast<int>(dice_string.size()); i++) {
    if (i >= error_position && i <= error_position + error_position_length - 1) {
      error_line += "^";
      continue;
    }

    error_line += "~";
  }

  spdlog::error("{}, {}", error_message, dice_string);
  reply(
    fmt::format("```diff\n- ERROR: {}\n  {}\n- {}\n```", error_message, dice_string, error_line));

  data.is_error = true;
}

std::string preprocess(const std::string input) {
  std::string result;
  result.reserve(input.size()); // reserve space to avoid reallocations
  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == 'd' && (i == 0 || !isdigit(input[i - 1]))) {
      result += "1d";
    } else {
      result += input[i];
    }
  }
  return result;
}

bool has_precedence(char op1, char op2) {
  if (op2 == '(' || op2 == ')') {
    return false;
  }
  if ((op1 == '*' || op1 == '/') && (op2 == '+' || op2 == '-')) {
    return false;
  }
  if (op1 == 'd') {
    return false;
  }
  return true;
}

int apply_operator(int op1, int op2, char op, roll_flags_d flags, roll_result_d *result) {
  switch (op) {
    case '+': return op1 + op2;
    case '-': return op1 - op2;
    case '*': return op1 * op2;
    case '/': return op1 / op2;
    case 'd':
      int total = 0;
      // Generate X random numbers between 1 and Y and add them together
      result->rolls.push_back({});
      for (int i = 0; i < op1; i++) {
        int value = rand() % op2 + 1;
        // Use result.total as a temporary variable since its already allocated.
        // It gets overwritten later anyways.
        total += value;
        result->rolls[result->rolls.size() - 1].push_back(value);
        result->result.push_back(value);
      }

      for (size_t i = 0; i < result->rolls.size(); i++) {
        for (size_t j = 0; j < result->rolls[i].size(); j++) {
          int value = result->rolls[i][j];
          size_t k = 0;
          while (value >= flags.reroll.first && value <= flags.reroll.second) {
            int new_value = rand() % op2 + 1;
            result->rolls[i][j] = new_value;
            result->reroll_positions.push_back(std::make_pair(i, j));

            if (flags.explode) {
              total += new_value;
            } else {
              total -= (value - new_value);
            }
            value = new_value;
            if (k >= std::min<size_t>(flags.reroll_repeat, 100) - 1) {
              break;
            }
            k += 1;
          }
          if (k > 0) {
            result->reroll_positions.push_back(std::make_pair(i, j));
          }
        }
      }

      for (size_t i = 0; i < result->rolls.size(); i++) {
        result->rolls[i].erase(std::remove(result->rolls[i].begin(), result->rolls[i].end(), 0),
                               result->rolls[i].end());
        result->rolls[i].resize(result->rolls[i].size());
      }

      std::sort(result->result.begin(), result->result.end(), std::greater<int>());

      if (flags.reverse_keep) {
        result->result.erase(result->result.begin(), result->result.end() - flags.keep);
      } else {
        result->result.erase(result->result.begin() + flags.keep, result->result.end());
      }

      return total;
  }

  return 'e';
}

int roll::evaluate_expression(std::string expression, roll_flags_d flags) {
  roll_result_d result = {};
  std::stack<int> operands;
  std::stack<char> operators;

  for (size_t i = 0; i < expression.length(); i++) {
    char c = expression[i];

    if (isdigit(c)) {
      int num = c - '0';
      while (i + 1 < expression.length() && isdigit(expression[i + 1])) {
        num = num * 10 + (expression[i + 1] - '0');
        i++;
      }
      operands.push(num);
    } else if (c == '(') {
      operators.push(c);
    } else if (c == ')') {
      while (operators.top() != '(') {
        int op2 = operands.top();
        operands.pop();
        int op1 = operands.top();
        operands.pop();
        char op = operators.top();
        operators.pop();

        for (int operand : {op1, op2}) {
          if (operand <= 0) {
            error(expression, "Number has to be larger than 0!", i);
            return 'e';
          }
          if (operand > 10000) {
            error(expression, "Number has to be smaller than 10k!", i);
            return 'e';
          }
        }

        int apply = apply_operator(op1, op2, op, flags, &result);

        if (apply == 'e') {
          roll::error(expression, "Invalid Character!", i);
          return 'e';
        }

        operands.push(apply);
      }
      operators.pop();
    } else {
      while (!operators.empty() && has_precedence(c, operators.top())) {
        int op2 = operands.top();
        operands.pop();
        int op1 = operands.top();
        operands.pop();
        char op = operators.top();
        operators.pop();
        int apply = apply_operator(op1, op2, op, flags, &result);
        if (apply == 'e') {
          return 'e';
        }
        operands.push(apply);
      }
      operators.push(c);
    }
  }

  while (!operators.empty()) {
    int op2 = operands.top();
    operands.pop();
    int op1 = operands.top();
    operands.pop();
    char op = operators.top();
    operators.pop();
    int apply = apply_operator(op1, op2, op, flags, &result);

    if (apply == 'e') {
      roll::error(expression, "Invalid Character!");
      return 'e';
    }
    operands.push(apply);
  }

  data.results.push_back(result);
  return operands.top();
}

roll_flags_d roll::parse_modifiers(std::string modifiers) {
  roll_flags_d flags;

  // Define the regular expression to match the rules
  boost::regex pattern(
    // Match zero or more digits, or an asterisk
    "(\\d*|\\*)"
    // Match 'e', 'r', "explode", or "reroll"
    "([er]|explode|reroll)"
    // Match one or more digits
    "(\\d+)"
    // Match an optional operator: either ">=", ">", "<=", "<", "-"
    "([<>]=?|-(?=\\d))?"
    // Match an optional digit string, if the operator is "-", with a lookbehind assertion
    "((?<=-)\\d*)?"
    // Boolean OR operator
    "|"
    // Match an optional "-" character
    "(-)?"
    // Match the "k" character
    "k"
    // Match one or more digits
    "(\\d+)");

  boost::sregex_iterator it(modifiers.begin(), modifiers.end(), pattern);
  boost::sregex_iterator end;
  // boost::regex_search(modifiers, matches, pattern);

  while (it != end) {
    boost::smatch matches = *it;

    std::string reroll_prefix = matches[1].str();
    auto reroll_digits = std::make_pair(matches[3].str(), matches[5].str());
    std::string reroll_operator = matches[4].str();
    std::string k_prefix = matches[6].str();
    std::string k_digit = matches[7].str();

    std::string reroll_type = matches[2].str();

    flags.explode = reroll_type[0] == 'e';

    if (!reroll_prefix.empty()) {
      try {
        flags.reroll_repeat = std::stoi(reroll_prefix);
        if (flags.reroll_repeat > 10000) {
          error(modifiers, "Number has to be less than 10k!");
          flags.is_error = true;
          return flags;
        } else if (flags.reroll_repeat < 0) {
          error(modifiers, "Number has to be greater than 0!");
          flags.is_error = true;
          return flags;
        }
      } catch (const std::invalid_argument &e) {
        flags.reroll_repeat = std::numeric_limits<int>::max();
      }
    }
    if (!reroll_operator.empty()) {
      wiz::string value = reroll_operator;
      bool mode_equals = false;
      if (value.contains('=')) {
        mode_equals = true;
      }

      if (value.contains('>')) {
        flags.reroll.first = std::stoi(reroll_digits.first) + !mode_equals;
        flags.reroll.second = std::numeric_limits<int>::max();
      } else if (value.contains('<')) {
        flags.reroll.first = -std::numeric_limits<int>::max();
        flags.reroll.second = std::stoi(reroll_digits.first) + !mode_equals;
      } else if (value.contains('-')) {
        flags.reroll.first = std::stoi(reroll_digits.first);
        flags.reroll.second = std::stoi(reroll_digits.second);
      }

      if (flags.reroll.first > 10000 ||
          (flags.reroll.second != std::numeric_limits<int>::max() && flags.reroll.second > 10000)) {
        error(modifiers, "Number has to be less than 10k!");
        flags.is_error = true;
        return flags;
      } else if ((flags.reroll.first != -std::numeric_limits<int>::max() &&
                  flags.reroll.first < 0) ||
                 flags.reroll.second < 0) {
        error(modifiers, "Number has to be greater than 0!");
        flags.is_error = true;
        return flags;
      }
    } else if (!reroll_digits.first.empty()) {
      flags.reroll.first = std::stoi(reroll_digits.first);
      if (flags.reroll.first > 10000) {
        error(modifiers, "Number has to be less than 10k!");
        flags.is_error = true;
        return flags;
      } else if (flags.reroll.first < 0) {
        error(modifiers, "Number has to be greater than 0!");
        flags.is_error = true;
        return flags;
      }
      flags.reroll.second = flags.reroll.first;
    }
    if (!k_prefix.empty()) {
      flags.reverse_keep = true;
    }
    if (!k_digit.empty()) {
      flags.keep = std::stoi(k_digit);
      if (flags.keep > 10000) {
        error(modifiers, "Number has to be less than 10k!");
        flags.is_error = true;
        return flags;
      } else if (flags.keep < 0) {
        error(modifiers, "Number has to be greater than 0!");
        flags.is_error = true;
        return flags;
      }
    }

    ++it;
  }

  return flags;
}

void roll::parse_dice_string(const wiz::string &input) {
  data = roll_d();

  wiz::string input_string = input;

  if (input == "stats")
    input_string = "6 4d6 k3";

  std::vector<wiz::string> vec = input_string.split(" ");

  int repeat = 1;
  wiz::string dice_string = "";
  wiz::string modifiers = "";

  if (vec.size() == 1) {
    dice_string = input_string;
  } else if (vec.size() == 2) {
    if (vec[0].is_int()) {
      repeat = std::stoi(vec[0]);
      dice_string = vec[1];
    } else {
      dice_string = vec[0];
      modifiers = vec[1];
    }
  } else {
    if (vec[0].is_int()) {
      repeat = std::stoi(vec[0]);
      dice_string = vec[1];
      for (auto it = std::next(vec.begin(), 2); it != vec.end(); ++it) {
        modifiers += *it;
      }
    } else {
      dice_string = vec[0];
      for (auto it = std::next(vec.begin(), 1); it != vec.end(); ++it) {
        modifiers += *it;
      }
    }
  }

  if (repeat > 10000)
    return error(input_string, "Number has to be smaller than 10k!", 0, vec[0].size());

  if (repeat <= 0)
    return error(input_string, "Number has to be larger than 0!", 0, vec[0].size());

  roll_flags_d flags;

  if (modifiers.size() > 0) {
    flags = parse_modifiers(modifiers);
    if (flags.is_error)
      return;
  }

  for (int i = 0; i < repeat; i++) {
    int total = evaluate_expression(preprocess(dice_string), flags);
    if (total == 'e')
      return;

    data.total += total;
    if (static_cast<int>(data.results.size()) > 0) {
      data.results[i].total = total;
    } else {
      roll_result_d result = {};
      result.total = total;
      data.results.push_back(result);
    }
  }
}

void roll::command_definition() {
  data = {};

  std::vector<dpp::command_data_option> options = event->command.get_command_interaction().options;

  std::string input_string = std::get<std::string>(options[0].value);

  parse_dice_string(input_string);

  if (data.is_error) {
    return;
  }

  std::stringstream output;
  std::string truncate_string = "[...]\n````truncated`";
  const size_t message_max_size = 2000;

  output << event->command.usr.get_mention() << " rolled `[" << input_string << "]`: `"
         << data.total << "`\n```js" << std::endl;

  size_t roll_size = 0;
  for (const roll_result_d &result : data.results) {
    for (const std::vector<int> &roll : result.rolls) {
      roll_size = std::max(roll_size, roll.size());
    }
  }

  int max_size_total = 0;
  int max_size_result = 0;
  std::vector<int> max_size_rolls;
  max_size_rolls.resize(roll_size, 0);
  for (const roll_result_d &result : data.results) {
    for (const std::vector<int> &rolls : result.rolls) {
      for (size_t i = 0; i < rolls.size(); i++) {
        int value_size = (int)std::to_string(rolls[i]).size();
        max_size_rolls[i] = std::max(max_size_rolls[i], value_size);
      }
    }
    for (const int &res : result.result) {
      max_size_result = std::max(max_size_result, (int)std::to_string(res).size());
    }
    max_size_total = std::max(max_size_total, (int)std::to_string(result.total).size());
  }

  std::streampos prev_pos = 0;
  for (roll_result_d result : data.results) {
    prev_pos = output.tellp();
    output << "Result " << std::setw(max_size_total) << std::right << result.total << " :";

    if (result.result.size() > 0) {
      output << " (";
      for (size_t i = 0; i < result.result.size(); ++i) {
        output << std::setw(max_size_result) << std::right << result.result[i];
        if (i < result.result.size() - 1) {
          output << ", ";
        }
      }
      output << ")";
    }

    for (size_t i = 0; i < result.rolls.size(); i++) {
      output << " [";
      for (size_t j = 0; j < result.rolls[i].size(); j++) {
        std::string value = std::to_string(result.rolls[i][j]);
        if (std::find(result.reroll_positions.begin(), result.reroll_positions.end(),
                      std::make_pair(i, j)) != result.reroll_positions.end()) {
          for (int k = value.size() - 1; k >= 0; --k) {
            value.insert(k, "͟");
          }
        }
        output << std::setw(max_size_rolls[j]) << std::right << value;
        if (j < result.rolls[i].size() - 1) {
          output << ", ";
        }
      }
      output << "]";
    }

    output << std::endl;

    if (static_cast<size_t>(output.tellp()) > (message_max_size - truncate_string.size())) {
      output.str(output.str().substr(0, prev_pos));
      output.seekp(prev_pos);
      output << truncate_string;
      prev_pos = -1;
      break;
    }
  }

  if (prev_pos != -1) {
    output << "```";
  }

  reply(output.str(), ", Message Size: " + std::to_string(output.str().size()));
}