#ifndef MUSICOS_FUCK_H
#define MUSICOS_FUCK_H

#include "musicos/random_command.h"

class fuck : public random_command {
public:
  fuck(dpp::snowflake bot_id) : random_command({"Me ;)", "You", "Off", "That", "It", "Yes", "This", "The Man"}) {
    command_interface = dpp::slashcommand("fuck", "Brandon?", bot_id);
  }
};

#endif // MUSICOS_FUCK_H