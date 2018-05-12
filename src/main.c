
#include "blackjack.h"

int main(int argc, char *argv[])
{
  struct argp argp = { options, parse_opt, "", doc, 0, 0, 0 };
  struct arguments arguments = { .players = 1 };
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  srand((unsigned)time(NULL));

  int mouse_x, mouse_y;
  bool quit = false;

  //SDL_StartTextInput();

  struct Game game = { .screen_h = SCREEN_H,
		       .screen_w = SCREEN_W,
		       .num_decks = 8,
		       .money = 10000,
		       .current_bet = 500,
		       .shuffle_specs = shuffle_specs,
		       .card_faces = card_faces,
		       .num_players = arguments.players,
		       .current_menu = MenuHand
  };
  
  SDL_Event event;
  SDL_Window *window = create_window(&game);
  game.renderer = create_renderer(window);
  game.cards_texture = load_cards_texture(game.renderer);

  SDL_Texture *bg_texture = load_bg_texture(game.renderer);
  SDL_Texture *rules_texture = load_rules_texture(game.renderer);
  
  load_btn_textures(&game);
  load_fonts(&game);
  load_game(&game);

  new_regular(&game);
  //new_aces(&game);
  //new_eights(&game);

  deal_new_hand(&game);
  
  while(!quit)
  {
    SDL_RenderClear(game.renderer);
    SDL_RenderCopy(game.renderer, bg_texture, NULL, NULL);
    SDL_RenderCopy(game.renderer, rules_texture, NULL, NULL);

    draw_dealer_hand(&game);
    draw_player_hands(&game);

    switch(game.current_menu)
    {
    case MenuHand:
      draw_hand_menu(&game);
      break;
    case MenuGame:
      draw_game_menu(&game);
      break;
    case MenuNewBet:
      draw_bet_menu(&game);
      break;
    }

    draw_money(&game);
    draw_bet(&game);

    SDL_RenderPresent(game.renderer);

    while(SDL_PollEvent(&event) != 0)
    {
      switch(event.type)
      {
      case SDL_MOUSEBUTTONUP:
	SDL_GetMouseState(&mouse_x, &mouse_y);
	handle_click(&game, &event.button, mouse_x, mouse_y);
	break;

      case SDL_QUIT:
	quit = true;
	break;
      }

      if (event.type == SDL_WINDOWEVENT)
      {
        switch(event.window.event)
	{
        case SDL_WINDOWEVENT_RESIZED:
	  /*
	  printf("Window %d resized to %dx%d\n",
		 event.window.windowID,
		 event.window.data1,
		 event.window.data2);
	  */
	  game.screen_w = (unsigned) event.window.data1;
	  game.screen_h = (unsigned) event.window.data2;
	  break;
	}
      }
    }

    SDL_Delay(30);
  }
  
  return 0;
}
