#ifndef MUSICOS_CRUSADE_TIME_H
#define MUSICOS_CRUSADE_TIME_H

#include "musicos/random_command.h"

class crusade_time : public random_command {
protected:
  std::vector<std::string> choices = {"DEUS VULT", "DUES VULT", "DORIME", "HABEMUS GLADII DOU",
                                      "SI VIS PACHEM PARA BELLUM"};

public:
  crusade_time(dpp::snowflake bot_id) {
    command_interface =
      dpp::slashcommand("crusade", "N/A", bot_id)
        .add_option(dpp::command_option(dpp::co_sub_command, "time", "Deus Vult"));
  }
};

#endif // MUSICOS_CRUSADE_TIME_H
