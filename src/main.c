
#include "blackjack.h"

int main(int argc, char *argv[])
{
  struct argp argp = { options, parse_opt, "", doc, 0, 0, 0 };
  struct arguments arguments = { .players = 1 };
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  srand((unsigned)time(NULL));

  struct Game game = { .num_decks = 8,
		       .money = 10000,
		       .current_bet = 500,
		       .shuffle_specs = shuffle_specs,
		       .card_faces = card_faces,
		       .num_players = arguments.players };

  load_game(&game);
  new_regular(&game);
  deal_new_hand(&game);

  int mouse_x, mouse_y;
  bool quit = false;

  SDL_Event event;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Surface *bg_surface = NULL;
  SDL_Texture *bg_texture = NULL;
  SDL_Surface *cards_surface = NULL;
  SDL_Texture *cards_texture = NULL;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  window = SDL_CreateWindow(
    "Blackjack",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_W,
    SCREEN_H,
    SDL_WINDOW_OPENGL
  );

  if(window == NULL)
  {
    printf("Could not create window: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL)
  {
    printf("Count not get renderer! SDL Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  bg_surface = SDL_LoadBMP("img/bg.bmp");
  if(bg_surface == NULL)
  {
    printf( "Unable to load image %s! SDL Error: %s\n", "img/bg.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
  SDL_FreeSurface(bg_surface);

  cards_surface = SDL_LoadBMP("img/cards.bmp");
  if(cards_surface == NULL)
  {
    printf( "Unable to load image %s! SDL Error: %s\n", "img/cards.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  cards_texture = SDL_CreateTextureFromSurface(renderer, cards_surface);
  SDL_FreeSurface(cards_surface);

  game.renderer = renderer;
  game.cards_texture = cards_texture;
  
  while(!quit)
  {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bg_texture, NULL, NULL);

    draw_hands(&game);
    
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
