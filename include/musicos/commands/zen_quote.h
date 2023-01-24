#ifndef MUSICOS_ZENQUOTE_H
#define MUSICOS_ZENQUOTE_H

#include "musicos/command.h"
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <string>

class zen_quote : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("zenquote", "Only the thiccest of quotes",
                             event->command.application_id);
  }

  void command_definition() override;
};

#endif // MUSICOS_ZENQUOTE_H