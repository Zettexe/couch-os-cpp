#ifndef MUSICOS_PASTA_H
#define MUSICOS_PASTA_H

#include "musicos/command.h"

class pasta : public command {
private:
  const char *recipe =
    R"(Ingredients: 160g Barilla Pasta, 50g Red bell pepper, 50g Leek, 50g Zucchini, 50g Eggplant, 40g Capers, 2 Tablespoons extra virgin olive oil, and Fresh basil

    Instructions:
    1. Wash and cut the leek into strips. 
    2. Cut the bell pepper, Zucchini, and eggplant into small cubes. 
    3. Wash and chop the capers. 
    4. Heat the olive oil in a pan and sauté the eggplants. Set aside. 
    5. Add more olive oil in the same pan and add in the rest of the vegetables; leek, red peppers, and zucchini. Add the chopped capers. 
    6. Boil the pasta in a large amount of slightly salted boiling water. 
    7. Drain the pasta, toss and sauté with the veggies. 
    8. Sprinkle the sautéed eggplants. 
    9. Drizzle with a little bit of olive oil, sprinkle fresh basil for the final touch and serve! 

    Enjoy! ^-^)";

public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("pasta", "Big tasty pasta food", event->command.application_id);
  }

  inline void command_definition() override {
    dpp::embed embed =
      dpp::embed()
        .set_title("Pasta Recipe")
        .set_author("Chef CouchOS", "",
                    "https://www.dropbox.com/s/4g0om3fb3vmne69/chef-couch-os.jpg?dl=1")
        .set_description(recipe);

    reply(
      dpp::message(event->command.get_channel().id, embed).set_reference(event->command.msg.id));
  }
};

#endif // MUSICOS_PASTA_H