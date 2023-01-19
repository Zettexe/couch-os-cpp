#ifndef MUSICOS_GTQUOTE_H
#define MUSICOS_GTQUOTE_H

#include "musicos/commands/quote.h"

class god_tier_quote : public quote {
public:
  god_tier_quote(dpp::snowflake bot_id) : quote() {
    command_interface =
      dpp::slashcommand("god", "god", bot_id)
        .add_option(
          dpp::command_option(dpp::co_sub_command_group, "tier", "tier").add_option(dpp::command_option(dpp::co_sub_command, "quote", "Gouda Tier Quotes")));

    update_quotes(std::filesystem::path("data/gtquotes.json"));
  }
};

#endif // MUSICOS_GTQUOTE_H