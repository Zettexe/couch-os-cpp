#include "musicos/commands/zen_quote.h"

void zen_quote::command_definition() {
  std::stringstream response;
  curlpp::Easy request;

  request.setOpt(curlpp::Options::Url("https://zenquotes.io/api/random"));
  request.setOpt(curlpp::Options::WriteStream(&response));
  request.perform();

  nlohmann::json data = nlohmann::json::parse(response.str())[0];

  std::string quote = data["q"].get<std::string>();
  std::string author = data["a"].get<std::string>();

  spdlog::info("Quote: {}, Author: {}", quote, author);

  // Check if the first and last characters of the strings are quotation marks
  // and remove them if they are
  if (quote.front() == '"') {
    quote.erase(0, 1);
  }
  if (quote.back() == '"') {
    quote.erase(quote.size() - 1, 1);
  }

  if (author.front() == '"') {
    author.erase(0, 1);
  }
  if (author.back() == '"') {
    author.erase(author.size() - 1, 1);
  }

  quote.insert(0, "*\"");
  quote.insert(quote.size(), "\"*");

  std::stringstream ss;
  ss << quote << std::endl << "-" << author;

  reply(ss.str());
}