#ifndef MUSICOS_FUCK_H
#define MUSICOS_FUCK_H

#include "musicos/random_command.h"

class fuck : public random_command {
protected:
  std::vector<std::string> choices = {"Me ;)", "You", "Off",  "That",
                                      "It",    "Yes", "This", "The Man"};

public:
  fuck(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("fuck", "Brandon?", bot_id); }
};

#endif // MUSICOS_FUCK_H