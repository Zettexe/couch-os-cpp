#ifndef MUSICOS_QUOTE_H
#define MUSICOS_QUOTE_H

#include "musicos/command.h"

struct quote_t {
  std::string text;
  std::string author;
};

class quote : public command {
private:
  std::vector<quote_t> quotes;
  std::filesystem::path path = "data/quotes.json";

public:
  dpp::slashcommand register_command() override {
    update_quotes(std::filesystem::path(path));

    return dpp::slashcommand("quote", "Citations", event->command.application_id)
      .add_option(dpp::command_option(dpp::co_string, "quote", "Quote to add", false))
      .add_option(dpp::command_option(dpp::co_string, "author", "Whodunnit", false));
  }

  void update_quotes(std::filesystem::path file_path);

  void add_quote(std::vector<dpp::command_data_option> options);

  void command_definition() override;
};

#endif // MUSICOS_QUOTE_H