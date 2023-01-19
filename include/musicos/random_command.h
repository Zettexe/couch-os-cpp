#ifndef MUSICOS_RANDOM_COMMAND_H
#define MUSICOS_RANDOM_COMMAND_H

#include "musicos/command.h"
#include <random>
#include <string>
#include <vector>

class random_command : public command {
private:
  std::vector<std::string> choices;

public:
  random_command(std::vector<std::string> c) : choices(c) {}

  std::string random_choice();

  void command_definition() override;
};

#endif // MUSICOS_RANDOM_COMMAND_H