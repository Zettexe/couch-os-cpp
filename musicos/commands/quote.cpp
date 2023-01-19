#include "musicos/commands/quote.h"

void quote::update_quotes(std::filesystem::path file_path) {
  std::ifstream in(file_path);
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string json_string = buffer.str();
  in.close();

  nlohmann::json quotes_json = nlohmann::json::parse(json_string);

  // Iterate through the quotes in the JSON object and add them to the array
  for (const nlohmann::json &quote : quotes_json) {
    quote_t q;
    q.text = quote.value("quote", "Hewwo I am bwoken, pwease report dis as a bug.");
    q.author = quote.value("author", "");
    quotes.push_back(q);
  }
}

void quote::add_quote(std::vector<dpp::command_data_option> options) {
  quote_t q;
  q.text = std::get<std::string>(options[0].value);
  if (options.size() > 1) {
    q.author = std::get<std::string>(options[1].value);
  }

  quotes.push_back(q);

  // Open the quotes.json file in write mode
  std::ofstream quotes_file(path, std::ios::out | std::ios::trunc);

  // Create a new JSON object to store the quotes
  nlohmann::json quotes_json;

  // Iterate through the quotes vector and add each quote to the JSON object
  for (const quote_t &quote : quotes) {
    nlohmann::json quote_json;
    quote_json["quote"] = quote.text;
    quote_json["author"] = quote.author;
    quotes_json.push_back(quote_json);
  }

  // Write the JSON object to the quotes.json file
  quotes_file << quotes_json.dump(4);
  quotes_file.close();

  reply("Quote added successfully.");
}

void quote::command_definition() {
  std::vector<dpp::command_data_option> options = event->command.get_command_interaction().options;

  if (!options.empty() && options[0].name == "quote") {
    add_quote(options);
    return;
  }

  if (quotes.empty()) {
    reply("No quotes available.");
    return;
  }

  // Generate a random index in the range [0, quotes.size() - 1]
  static std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, quotes.size() - 1);
  std::size_t index = dist(rng);

  // Get the quote and author at the random index
  quote_t quote = quotes[index];

  quote.text.insert(0, "*\"").insert(quote.text.size(), "\"*");

  if (quote.author != "") {
    reply(quote.text + "\n-" + quote.author);
  } else {
    reply(quote.text);
  }
}