#ifndef MUSICOS_GTQUOTE_H
#define MUSICOS_GTQUOTE_H

#include "musicos/commands/quote.h"

class god_tier_quote : public quote {
public:
  dpp::slashcommand register_command() override {
    update_quotes(std::filesystem::path("data/gtquotes.json"));

    return dpp::slashcommand("god", "god", event->command.application_id)
      .add_option(
        dpp::command_option(dpp::co_sub_command_group, "tier", "tier")
          .add_option(dpp::command_option(dpp::co_sub_command, "quote", "Gouda Tier Quotes")));
  }
};

#endif // MUSICOS_GTQUOTE_H