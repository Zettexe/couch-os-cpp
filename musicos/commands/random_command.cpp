#include "musicos/random_command.h"

std::string random_command::random_choice() {
  static std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, choices.size() - 1);
  return choices[dist(rng)];
}

void random_command::command_definition() { reply(random_choice()); }