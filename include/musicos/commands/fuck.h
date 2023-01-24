#ifndef MUSICOS_FUCK_H
#define MUSICOS_FUCK_H

#include "musicos/random_command.h"

class fuck : public random_command {
protected:
  std::vector<std::string> choices = {"Me ;)", "You", "Off",  "That",
                                      "It",    "Yes", "This", "The Man"};

public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("fuck", "Brandon?", event->command.application_id);
  }
};

#endif // MUSICOS_FUCK_H