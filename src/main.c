
#include "blackjack.h"

int main(int argc, char *argv[])
{
  struct argp argp = { options, parse_opt, "", doc, 0, 0, 0 };
  struct arguments arguments = { .players = 1 };
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  srand((unsigned)time(NULL));

  int mouse_x, mouse_y;
  bool quit = false;

  SDL_Event event;
  SDL_Window *window = create_window();
  SDL_Renderer *renderer = create_renderer(window);
  SDL_Texture *bg_texture = load_bg_texture(renderer);
  SDL_Texture *rules_texture = load_rules_texture(renderer);
  SDL_Texture *cards_texture = load_cards_texture(renderer);

  struct Game game = { .num_decks = 8,
		       .money = 10000,
		       .current_bet = 500,
		       .shuffle_specs = shuffle_specs,
		       .card_faces = card_faces,
		       .num_players = arguments.players,
		       .renderer = renderer,
		       .cards_texture = cards_texture,
		       .current_menu = MenuHand
  };

  load_btn_textures(&game, renderer);
  
  load_game(&game);
  //new_regular(&game);
  new_aces(&game);
  deal_new_hand(&game);
  
  while(!quit)
  {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bg_texture, NULL, NULL);
    SDL_RenderCopy(renderer, rules_texture, NULL, NULL);

    draw_hands(&game);

    switch(game.current_menu)
    {
    case MenuHand:
      draw_hand_menu(&game);
      break;
    case MenuGame:
      draw_game_menu(&game);
      break;
    }
    
    SDL_RenderPresent(renderer);

    while(SDL_PollEvent(&event) != 0)
    {
      if(event.type == SDL_MOUSEBUTTONUP)
      {
	SDL_GetMouseState(&mouse_x, &mouse_y);
	handle_click(&game, &event.button, mouse_x, mouse_y);
      }

      if(event.type == SDL_QUIT)
      {
	quit = true;
      }
    }

    SDL_Delay(30);
  }
  
  return 0;
}
