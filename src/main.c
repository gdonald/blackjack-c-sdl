
#include "blackjack.h"

int main(void)
{
  srand((unsigned)time(NULL));

  int mouse_x, mouse_y;
  bool quit = false;

  struct Game game = { .screen_h = SCREEN_H,
		       .screen_w = SCREEN_W,
		       .player_hands_y_offset = PLAYER_HANDS_Y_OFFSET,
		       .buttons_y_offset = BUTTONS_Y_OFFSET,
		       .num_decks = 8,
		       .money = 10000,
		       .current_bet = 500,
		       .shuffle_specs = shuffle_specs,
		       .current_menu = MenuHand
  };

  game.player_hands_y_percent = (unsigned)((float) game.player_hands_y_offset / (float) game.screen_h * 100.0f);
  game.buttons_y_percent      = (unsigned)((float) game.buttons_y_offset      / (float) game.screen_h * 100.0f);

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
  deal_new_hand(&game);
  
  while(!quit)
  {
    SDL_RenderClear(game.renderer);
    SDL_RenderCopy(game.renderer, bg_texture, NULL, NULL);
    SDL_RenderCopy(game.renderer, rules_texture, NULL, NULL);

    draw_dealer_hand(&game);
    draw_player_hands(&game);
    draw_menus(&game);
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

      case SDL_KEYDOWN:
	switch(event.key.keysym.sym)
	{
	case SDLK_BACKSPACE:
	  switch(game.current_menu)
	  {
	  case MenuNewBet:
	    if(strlen(game.current_bet_str))
	    {
	      game.current_bet_str[strlen(game.current_bet_str) - 1] = '\0';
	    }
	    break;

	  case MenuNewNumDecks:
	    if(strlen(game.num_decks_str))
	    {
	      game.num_decks_str[strlen(game.num_decks_str) - 1] = '\0';
	    } 
	    break;
	  }

          break;

	case SDLK_RETURN:
	  switch(game.current_menu)
	  {
	  case MenuNewBet:
	    game.current_bet = (unsigned) atoi(game.current_bet_str) * 100;
	    normalize_bet(&game);
	    game.current_bet_str[0] = '\0';
	    game.current_menu = MenuHand;
	    deal_new_hand(&game);
	    break;

	  case MenuNewNumDecks:
	    game.num_decks = (unsigned) atoi(game.num_decks_str);
	    normalize_num_decks(&game);
	    shuffle(&game.shoe);
	    game.num_decks_str[0] = '\0';
	    game.current_menu = MenuHand;
	    deal_new_hand(&game);
	    break;
	  }
	  
	  SDL_StopTextInput();
	  break;
	}

      case SDL_TEXTINPUT:
	switch(game.current_menu)
	{
	case MenuNewBet:
	  handle_new_bet_keystroke(&game, event.text.text);
	  break;

	case MenuNewNumDecks:
	  handle_new_num_decks_keystroke(&game, event.text.text);
	  break;
	}

	break;

      case SDL_QUIT:
	quit = true;
	break;
      }

      if(event.type == SDL_WINDOWEVENT)
      {
        switch(event.window.event)
	{
        case SDL_WINDOWEVENT_RESIZED:
	  game.screen_w = (unsigned) event.window.data1;
	  game.screen_h = (unsigned) event.window.data2;
	  calculate_offsets(&game);
	  break;
	}
      }
    }

    SDL_Delay(30);
  }
  
  return 0;
}
