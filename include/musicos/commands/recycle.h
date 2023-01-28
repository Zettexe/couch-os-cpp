#ifndef MUSICOS_RECYCLE_H
#define MUSICOS_RECYCLE_H

#include "musicos/random_command.h"

class recycle : public random_command {
protected:
  std::vector<std::string> choices = {
    "Recycling is a concept. ♻️",
    "Make used new again.\nReuse, renew, recycle.\nKeep Earth beautiful.\n-kathy555",
    "Plastic recycling is a lie!"};

public:
  recycle(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("recycle", "Plastic recycling is a lie!", bot_id);
  }
};

#endif // MUSICOS_RECYCLE_H