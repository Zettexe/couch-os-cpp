#ifndef MUSICOS_RANDOM_COMMAND_H
#define MUSICOS_RANDOM_COMMAND_H

#include "musicos/command.h"
#include <random>

class random_command : public command {
private:
  std::string random_choice() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, choices.size() - 1);
    return choices[dist(rng)];
  }

protected:
  std::vector<std::string> choices;

public:
  void command_definition() override { reply(random_choice()); }
};

#endif // MUSICOS_RANDOM_COMMAND_H