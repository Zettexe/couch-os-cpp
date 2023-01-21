#ifndef MUSICOS_CRUSADE_TIME_H
#define MUSICOS_CRUSADE_TIME_H

#include "musicos/random_command.h"

class crusade_time : public random_command {
protected:
  std::vector<std::string> choices = {"DEUS VULT", "DUES VULT", "DORIME", "HABEMUS GLADII DOU",
                                      "SI VIS PACHEM PARA BELLUM"};

public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("crusade", "N/A", event->command.application_id)
      .add_option(dpp::command_option(dpp::co_sub_command, "time", "Deus Vult"));
  }
};

#endif // MUSICOS_CRUSADE_TIME_H
