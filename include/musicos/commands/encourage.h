#ifndef MUSICOS_ENCOURAGE_H
#define MUSICOS_ENCOURAGE_H

#include "musicos/random_command.h"

class encourage : public random_command {
  encourage(dpp::snowflake bot_id) :
      random_command({"You can do it!", "You got this!", "I believe in you!"}) {
    command_interface = dpp::slashcommand("encourage", "Positive vibes all around", bot_id);
  }
};

#endif // MUSICOS_ENCOURAGE_H