
#include "blackjack.h"

const char *argp_program_version = "blackjack 0.1";
const char *argp_program_bug_address = "https://github.com/gdonald/blackjack-c-sdl/issues";
const char *doc = "blackjack -- a simple blackjack program";

struct argp_option options[] = {
  {"players", 'p', "NUM", 0, "Number of players", 0 },
  { 0, 0, 0, 0, 0, 0 }
};

const unsigned shuffle_specs[8][2] = { { 95, 8 },
				       { 92, 7 },
				       { 89, 6 },
				       { 86, 5 },
				       { 84, 4 },
				       { 82, 3 },
				       { 81, 2 },
				       { 80, 1 } };

const char *const card_faces[14][4] = { { "🂡", "🂱", "🃁", "🃑" },
					{ "🂢", "🂲", "🃂", "🃒" },
					{ "🂣", "🂳", "🃃", "🃓" },
					{ "🂤", "🂴", "🃄", "🃔" },
					{ "🂥", "🂵", "🃅", "🃕" },
					{ "🂦", "🂶", "🃆", "🃖" },
					{ "🂧", "🂷", "🃇", "🃗" },
					{ "🂨", "🂸", "🃈", "🃘" },
					{ "🂩", "🂹", "🃉", "🃙" },
					{ "🂪", "🂺", "🃊", "🃚" },
					{ "🂫", "🂻", "🃋", "🃛" },
					{ "🂭", "🂽", "🃍", "🃝" },
					{ "🂮", "🂾", "🃎", "🃞" },
					{ "🂠", "",  "",  ""  } };

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *a = state->input;

  switch(key)
  {
  case 'p':
    if(*arg == '0') a->players = 0;
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

void draw_money(const struct Game *game)
{
  char str[32];
  sprintf(str, "Money: $%.2f", (double)(game->money / 100));
  write_text(game, str, FontLg, 7, 5);
}

void draw_bet(const struct Game *game)
{
  char str[32];
  sprintf(str, "Current Bet: $%.2f", (double)(game->current_bet / 100));
  write_text(game, str, FontLg, 7, 28);
}

void write_text(const struct Game *game, const char *text, const int font_size, const int x, const int y)
{
  SDL_Color color = { 255, 255, 255, 0 };
  int w, h;

  SDL_Surface* surface = TTF_RenderText_Blended(game->fonts[font_size], text, color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, surface);

  TTF_SizeText(game->fonts[font_size], text, &w, &h);
  SDL_Rect rect = { .x = x, .y = y, .w = w, .h = h };

  SDL_RenderCopy(game->renderer, texture, NULL, &rect);
}

bool inside_rect(SDL_Rect rect, int x, int y)
{
  return x > rect.x &&
         x < rect.x + rect.w &&
         y > rect.y &&
         y < rect.y + rect.h;
}

void handle_click(struct Game *game, SDL_MouseButtonEvent *button, int mouse_x, int mouse_y)
{
  if(button->button == SDL_BUTTON_LEFT)
  {
    if(game->current_menu == MenuHand)
    {
      if(inside_rect(game->btn_rects[BtnHit], mouse_x, mouse_y))
      {
	player_hit(game);
	return;
      }

      if(inside_rect(game->btn_rects[BtnStand], mouse_x, mouse_y))
      {
	player_stand(game);
	return;
      }
      
      if(inside_rect(game->btn_rects[BtnDbl], mouse_x, mouse_y))
      {
	player_dbl(game);
	return;
      }
      
      if(inside_rect(game->btn_rects[BtnSplit], mouse_x, mouse_y))
      {
	player_split(game);
	return;
      }
    }
    else if(game->current_menu == MenuGame)
    {
      if(inside_rect(game->btn_rects[BtnDeal], mouse_x, mouse_y))
      {
	deal_new_hand(game);	
	return;
      }

      if(inside_rect(game->btn_rects[BtnBet], mouse_x, mouse_y))
      {
	get_new_bet(game);	
	return;
      }

      if(inside_rect(game->btn_rects[BtnOptions], mouse_x, mouse_y))
      {
	game_options(game);	
	return;
      }

      if(inside_rect(game->btn_rects[BtnQuit], mouse_x, mouse_y))
      {
	exit(EXIT_SUCCESS);
      }
    }
  }
}

void draw_hand_menu(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  SDL_Rect clip[8];
  clip[BtnDbl].x = 0;
  clip[BtnDbl].y = player_can_dbl(game) ? BtnUp : BtnOff;
  clip[BtnDbl].w = BTN_W;
  clip[BtnDbl].h = BTN_H;

  clip[BtnHit].x = 0;
  clip[BtnHit].y = player_can_hit(player_hand) ? BtnUp : BtnOff;
  clip[BtnHit].w = BTN_W;
  clip[BtnHit].h = BTN_H;

  clip[BtnStand].x = 0;
  clip[BtnStand].y = player_can_stand(player_hand) ? BtnUp : BtnOff;
  clip[BtnStand].w = BTN_W;
  clip[BtnStand].h = BTN_H;

  clip[BtnSplit].x = 0;
  clip[BtnSplit].y = player_can_split(game) ? BtnUp : BtnOff;
  clip[BtnSplit].w = BTN_W;
  clip[BtnSplit].h = BTN_H;

  game->btn_rects[BtnDbl].x = (SCREEN_W / 2) - (BTN_W * 2) - (BTN_SPACE) - (BTN_SPACE / 2);
  game->btn_rects[BtnDbl].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnDbl].w = BTN_W;
  game->btn_rects[BtnDbl].h = BTN_H;

  game->btn_rects[BtnHit].x = (SCREEN_W / 2) - BTN_W - (BTN_SPACE / 2);
  game->btn_rects[BtnHit].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnHit].w = BTN_W;
  game->btn_rects[BtnHit].h = BTN_H;

  game->btn_rects[BtnStand].x = (SCREEN_W / 2) + (BTN_SPACE / 2);
  game->btn_rects[BtnStand].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnStand].w = BTN_W;
  game->btn_rects[BtnStand].h = BTN_H;

  game->btn_rects[BtnSplit].x = (SCREEN_W / 2) + BTN_W + (BTN_SPACE) + (BTN_SPACE / 2);
  game->btn_rects[BtnSplit].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnSplit].w = BTN_W;
  game->btn_rects[BtnSplit].h = BTN_H;
  
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnDbl],   &clip[BtnDbl],   &game->btn_rects[BtnDbl]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnHit],   &clip[BtnHit],   &game->btn_rects[BtnHit]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnStand], &clip[BtnStand], &game->btn_rects[BtnStand]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnSplit], &clip[BtnSplit], &game->btn_rects[BtnSplit]);

  game->current_menu = MenuHand;
}

void draw_game_menu(struct Game *game)
{
  SDL_Rect clip[8];
  clip[BtnDeal].x = 0;
  clip[BtnDeal].y = BtnUp;
  clip[BtnDeal].w = BTN_W;
  clip[BtnDeal].h = BTN_H;

  clip[BtnBet].x = 0;
  clip[BtnBet].y = BtnUp;
  clip[BtnBet].w = BTN_W;
  clip[BtnBet].h = BTN_H;

  clip[BtnOptions].x = 0;
  clip[BtnOptions].y = BtnUp;
  clip[BtnOptions].w = BTN_W;
  clip[BtnOptions].h = BTN_H;

  clip[BtnQuit].x = 0;
  clip[BtnQuit].y = BtnUp;
  clip[BtnQuit].w = BTN_W;
  clip[BtnQuit].h = BTN_H;

  game->btn_rects[BtnDeal].x = (SCREEN_W / 2) - (BTN_W * 2) - (BTN_SPACE) - (BTN_SPACE / 2);
  game->btn_rects[BtnDeal].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnDeal].w = BTN_W;
  game->btn_rects[BtnDeal].h = BTN_H;

  game->btn_rects[BtnBet].x = (SCREEN_W / 2) - BTN_W - (BTN_SPACE / 2);
  game->btn_rects[BtnBet].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnBet].w = BTN_W;
  game->btn_rects[BtnBet].h = BTN_H;

  game->btn_rects[BtnOptions].x = (SCREEN_W / 2) + (BTN_SPACE / 2);
  game->btn_rects[BtnOptions].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnOptions].w = BTN_W;
  game->btn_rects[BtnOptions].h = BTN_H;

  game->btn_rects[BtnQuit].x = (SCREEN_W / 2) + BTN_W + (BTN_SPACE) + (BTN_SPACE / 2);
  game->btn_rects[BtnQuit].y = BUTTONS_Y_OFFSET;
  game->btn_rects[BtnQuit].w = BTN_W;
  game->btn_rects[BtnQuit].h = BTN_H;

  SDL_RenderCopy(game->renderer, game->btn_textures[BtnDeal],    &clip[BtnDeal],    &game->btn_rects[BtnDeal]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnBet],     &clip[BtnBet],     &game->btn_rects[BtnBet]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnOptions], &clip[BtnOptions], &game->btn_rects[BtnOptions]);
  SDL_RenderCopy(game->renderer, game->btn_textures[BtnQuit],    &clip[BtnQuit],    &game->btn_rects[BtnQuit]);

  game->current_menu = MenuGame;
}

void load_btn_textures(struct Game *game, SDL_Renderer *renderer)
{
  SDL_Surface *btn_hit_surface     = SDL_LoadBMP("img/btn_hit.bmp");
  SDL_Surface *btn_split_surface   = SDL_LoadBMP("img/btn_split.bmp");
  SDL_Surface *btn_stand_surface   = SDL_LoadBMP("img/btn_stand.bmp");
  SDL_Surface *btn_dbl_surface     = SDL_LoadBMP("img/btn_dbl.bmp");
  SDL_Surface *btn_deal_surface    = SDL_LoadBMP("img/btn_deal.bmp");
  SDL_Surface *btn_bet_surface     = SDL_LoadBMP("img/btn_bet.bmp");
  SDL_Surface *btn_options_surface = SDL_LoadBMP("img/btn_options.bmp");
  SDL_Surface *btn_quit_surface    = SDL_LoadBMP("img/btn_quit.bmp");

  if(btn_hit_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_hit.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_split_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_split.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_stand_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_stand.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_dbl_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_dbl.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_deal_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_deal.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_bet_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_bet.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_options_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_options.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if(btn_quit_surface == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "img/btn_quit.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  game->btn_textures[BtnHit]     = SDL_CreateTextureFromSurface(renderer, btn_hit_surface);
  game->btn_textures[BtnSplit]   = SDL_CreateTextureFromSurface(renderer, btn_split_surface);
  game->btn_textures[BtnStand]   = SDL_CreateTextureFromSurface(renderer, btn_stand_surface);
  game->btn_textures[BtnDbl]     = SDL_CreateTextureFromSurface(renderer, btn_dbl_surface);
  game->btn_textures[BtnDeal]    = SDL_CreateTextureFromSurface(renderer, btn_deal_surface);
  game->btn_textures[BtnBet]     = SDL_CreateTextureFromSurface(renderer, btn_bet_surface);
  game->btn_textures[BtnOptions] = SDL_CreateTextureFromSurface(renderer, btn_options_surface);
  game->btn_textures[BtnQuit]    = SDL_CreateTextureFromSurface(renderer, btn_quit_surface);

  SDL_FreeSurface(btn_hit_surface);
  SDL_FreeSurface(btn_stand_surface);
  SDL_FreeSurface(btn_dbl_surface);
  SDL_FreeSurface(btn_split_surface);
  SDL_FreeSurface(btn_deal_surface);
  SDL_FreeSurface(btn_bet_surface);
  SDL_FreeSurface(btn_options_surface);
  SDL_FreeSurface(btn_quit_surface);
}

SDL_Texture *load_cards_texture(SDL_Renderer *renderer)
{
  SDL_Surface *cards_surface = SDL_LoadBMP("img/cards.bmp");
  if(cards_surface == NULL)
  {
    printf( "Unable to load image %s! SDL Error: %s\n", "img/cards.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Texture *cards_texture = SDL_CreateTextureFromSurface(renderer, cards_surface);
  SDL_FreeSurface(cards_surface);

  return cards_texture;
}

SDL_Texture *load_bg_texture(SDL_Renderer *renderer)
{
  SDL_Surface *bg_surface = SDL_LoadBMP("img/bg.bmp");
  if(bg_surface == NULL)
  {
    printf( "Unable to load image %s! SDL Error: %s\n", "img/bg.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Texture *bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
  SDL_FreeSurface(bg_surface);

  return bg_texture;
}

SDL_Texture *load_rules_texture(SDL_Renderer *renderer)
{
  SDL_Surface *rules_surface = SDL_LoadBMP("img/rules.bmp");
  if(rules_surface == NULL)
  {
    printf( "Unable to load image %s! SDL Error: %s\n", "img/rules.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Texture *rules_texture = SDL_CreateTextureFromSurface(renderer, rules_surface);
  SDL_FreeSurface(rules_surface);

  return rules_texture;
}

SDL_Renderer *create_renderer(SDL_Window *window)
{
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL)
  {
    printf("Count not get renderer! SDL Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return renderer;
}

SDL_Window *create_window(void)
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Window *window = SDL_CreateWindow(
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

  return window;
}

void draw_card(const struct Game *game, const struct Card *card, const unsigned x, const unsigned y)
{  
  SDL_Rect clip[1];
  clip[0].x = ((int) card->value * CARD_BMAP_SPACING) + ((int) card->value * CARD_W);
  clip[0].y = ((int) card->suit  * CARD_BMAP_SPACING) + ((int) card->suit  * CARD_H);
  clip[0].w = CARD_W;
  clip[0].h = CARD_H;

  SDL_Rect offset = { (int) x, (int) y, CARD_W, CARD_H };

  SDL_RenderCopy(game->renderer, game->cards_texture, &clip[0], &offset);
}

bool is_ace(const struct Card *card)
{
  return card->value == 0;
}

bool is_ten(const struct Card *card)
{
  return card->value > 8;
}

unsigned player_get_value(const struct PlayerHand *player_hand, enum CountMethod method)
{
  unsigned v = 0;
  unsigned total = 0;
  unsigned tmp_v;

  for(unsigned x = 0; x < player_hand->hand.num_cards; ++x)
  {
    tmp_v = player_hand->hand.cards[x].value + 1;
    v = tmp_v > 9 ? 10 : tmp_v;

    if(method == Soft && v == 1 && total < 11)
    {
      v = 11;
    }

    total += v;
  }

  if(method == Soft && total > 21)
  {
    return player_get_value(player_hand, Hard);
  }

  return total;
}

bool player_is_busted(const struct PlayerHand *player_hand)
{
  return player_get_value(player_hand, Soft) > 21;
}

bool is_blackjack(const struct Hand *hand)
{
  if(hand->num_cards != 2)
  {
    return false;
  }

  if(is_ace(&hand->cards[0]) && is_ten(&hand->cards[1]))
  {
    return true;
  }

  if(is_ace(&hand->cards[1]) && is_ten(&hand->cards[0]))
  {
    return true;
  }

  return false;
}

bool player_can_hit(const struct PlayerHand *player_hand)
{
  if(player_hand->played
     || player_hand->stood
     || 21 == player_get_value(player_hand, Hard)
     || is_blackjack(&player_hand->hand)
     || player_is_busted(player_hand))
  {
    return false;
  }

  return true;
}

bool player_can_stand(const struct PlayerHand *player_hand)
{
  if(player_hand->stood
     || player_is_busted(player_hand)
     || is_blackjack(&player_hand->hand))
  {
    return false;
  }

  return true;
}

unsigned all_bets(const struct Game *game)
{
  unsigned bets = 0;

  for(unsigned x = 0; x < game->total_player_hands; x++)
  {
    bets += game->player_hands[x].bet;
  }

  return bets;
}

bool player_can_split(const struct Game *game)
{ 
  const struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  if(player_hand->stood || game->total_player_hands >= MAX_PLAYER_HANDS)
  {
    return false;
  }

  if(game->money < all_bets(game) + player_hand->bet)
  {
    return false;
  }

  if(player_hand->hand.num_cards == 2 && player_hand->hand.cards[0].value == player_hand->hand.cards[1].value)
  {
    return true;
  }

  return false;
}

bool player_can_dbl(const struct Game *game)
{
  const struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  if(game->money < all_bets(game) + player_hand->bet)
  {
    return false;
  }

  if(player_hand->stood
     || player_hand->hand.num_cards != 2
     || player_is_busted(player_hand)
     || is_blackjack(&player_hand->hand))
  {
    return false;
  }

  return true;
}

void deal_card(struct Shoe *shoe, struct Hand *hand)
{
  hand->cards[hand->num_cards++] = shoe->cards[shoe->current_card++];
}

bool player_is_done(struct Game *game, struct PlayerHand *player_hand)
{
  if(player_hand->played
     || player_hand->stood
     || is_blackjack(&player_hand->hand)
     || player_is_busted(player_hand)
     || 21 == player_get_value(player_hand, Soft)
     || 21 == player_get_value(player_hand, Hard))
  {
    player_hand->played = true;

    if(!player_hand->payed && player_is_busted(player_hand))
    {
      player_hand->payed = true;
      player_hand->status = Lost;
      game->money -= player_hand->bet;
    }

    return true;
  }

  return false;
}

bool more_hands_to_play(const struct Game *game)
{
  return game->current_player_hand < game->total_player_hands - 1;
}

bool need_to_play_dealer_hand(const struct Game *game)
{
  const struct PlayerHand *player_hand;

  for(unsigned x = 0; x < game->total_player_hands; ++x)
  {
    player_hand = &game->player_hands[x];

    if(!(player_is_busted(player_hand) || is_blackjack(&player_hand->hand)))
    {
      return true;
    }
  }

  return false;
}

unsigned dealer_get_value(const struct DealerHand *dealer_hand, enum CountMethod method)
{
  unsigned v = 0;
  unsigned total = 0;
  unsigned tmp_v;

  for(unsigned x = 0; x < dealer_hand->hand.num_cards; ++x)
  {
    if(x == 1 && dealer_hand->hide_down_card)
    {
      continue;
    }

    tmp_v = dealer_hand->hand.cards[x].value + 1;
    v = tmp_v > 9 ? 10 : tmp_v;

    if(method == Soft && v == 1 && total < 11)
    {
      v = 11;
    }

    total += v;
  }

  if(method == Soft && total > 21)
  {
    return dealer_get_value(dealer_hand, Hard);
  }

  return total;
}

bool dealer_is_busted(const struct DealerHand *dealer_hand)
{
  return dealer_get_value(dealer_hand, Soft) > 21;
}

void normalize_bet(struct Game *game)
{
  if(game->current_bet < MIN_BET)
  {
    game->current_bet = MIN_BET;
  }
  else if(game->current_bet > MAX_BET)
  {
    game->current_bet = MAX_BET;
  }
  
  if(game->current_bet > game->money)
  {
    game->current_bet = game->money;
  }
}

void save_game(const struct Game *game)
{
  FILE *fp = fopen(SAVE_FILE, "w");

  if(fp != NULL)
  {
    fprintf(fp, "%u\n%u\n%u\n", game->num_decks, game->money, game->current_bet);
    fclose(fp);
  }
}

void load_game(struct Game *game)
{
  FILE *fp = fopen(SAVE_FILE, "r");
  char buffer[32];

  if(fp != NULL)
  {
    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%u", &game->num_decks);

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%u", &game->money);

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%u", &game->current_bet);

    fclose(fp);
  }
}

void pay_hands(struct Game *game)
{
  struct DealerHand *dealer_hand = &game->dealer_hand;
  unsigned dhv = dealer_get_value(dealer_hand, Soft);
  bool dhb = dealer_is_busted(dealer_hand);
  struct PlayerHand *player_hand;
  unsigned phv;

  for(unsigned x = 0; x < game->total_player_hands; ++x)
  {
    player_hand = &game->player_hands[x];

    if(player_hand->payed)
    {
      continue;
    }

    player_hand->payed = true;

    phv = player_get_value(player_hand, Soft);

    if(dhb || phv > dhv)
    {
      if(is_blackjack(&player_hand->hand))
      {
	player_hand->bet *= 1.5;
      }

      game->money += player_hand->bet;
      player_hand->status = Won;
    }
    else if(phv < dhv)
    {
      game->money -= player_hand->bet;
      player_hand->status = Lost;
    }
    else
    {
      player_hand->status = Push;
    }
  }

  normalize_bet(game);
  save_game(game);
}

void play_dealer_hand(struct Game *game)
{
  struct DealerHand *dealer_hand = &game->dealer_hand;
  unsigned soft_count;
  unsigned hard_count;

  if(is_blackjack(&dealer_hand->hand))
  {
    dealer_hand->hide_down_card = false;
  }

  if(!need_to_play_dealer_hand(game))
  {
    dealer_hand->played = true;
    pay_hands(game);
    return;
  }

  dealer_hand->hide_down_card = false;

  soft_count = dealer_get_value(dealer_hand, Soft);
  hard_count = dealer_get_value(dealer_hand, Hard);

  while(soft_count < 18 && hard_count < 17)
  {
    deal_card(&game->shoe, &dealer_hand->hand);
    soft_count = dealer_get_value(dealer_hand, Soft);
    hard_count = dealer_get_value(dealer_hand, Hard);
  }

  dealer_hand->played = true;
  pay_hands(game);
}

void draw_dealer_hand(const struct Game *game)
{
  const struct DealerHand *dealer_hand = &game->dealer_hand;
  const struct Card *card;

  unsigned x_offset = (SCREEN_W / 2) - ((((dealer_hand->hand.num_cards - 1) * CARD_DRAW_SPACING) + CARD_W) / 2);

  for(unsigned i = 0; i < dealer_hand->hand.num_cards; ++i)
  {
    if(i == 1 && dealer_hand->hide_down_card)
    {
      struct Card c = { .value = 1, .suit = 4 };
      draw_card(game, &c, x_offset + (i * CARD_DRAW_SPACING), DEALER_HAND_Y_OFFSET);
    }
    else
    {
      card = &dealer_hand->hand.cards[i];
      draw_card(game, card, x_offset + (i * CARD_DRAW_SPACING), DEALER_HAND_Y_OFFSET);
    }
  }
}

void draw_player_hands(const struct Game *game)
{
  unsigned x_offset, x, h, c, hands_w = 0;
  char str[32], plus_minus[1], current[2];

  const struct PlayerHand *player_hand;
  const struct Card *card;
  
  for(h = 0; h < game->total_player_hands; h++)
  {
    player_hand = &game->player_hands[h];

    for(c = 0; c < player_hand->hand.num_cards - 1; c++)
    {
      hands_w += CARD_DRAW_SPACING;
    }

    hands_w += CARD_W;

    if(h < game->total_player_hands - 1)
    {
      hands_w += HAND_DRAW_SPACING;
    }
  }

  x_offset = (SCREEN_W / 2) - (hands_w / 2);
  x = x_offset;

  for(h = 0; h < game->total_player_hands; h++)
  {
    player_hand = &game->player_hands[h];

    if(player_hand->status == Lost)
    {
      sprintf(plus_minus, "-");
    }
    else if(player_hand->status == Won)
    {
      sprintf(plus_minus, "+");
    }
    else
    {
      sprintf(plus_minus, "");
    }

    if(!player_hand->played && h == game->current_player_hand)
    {
      sprintf(current, " *");
    }
    else
    {
      sprintf(current, "");
    }

    for(c = 0; c < player_hand->hand.num_cards; c++)
    {
      card = &player_hand->hand.cards[c];
      draw_card(game, card, x, PLAYER_HANDS_Y_OFFSET);

      if(c == 0)
      {
	sprintf(str, "%s$%.2f%s", plus_minus, (double)(player_hand->bet / 100), current);
	write_text(game, str, FontMd, (int) x + 3, PLAYER_HANDS_Y_OFFSET + CARD_H + 2);
      }

      if(c < player_hand->hand.num_cards - 1)
      {
	x += CARD_DRAW_SPACING;
      }
      else if(c == player_hand->hand.num_cards - 1)
      {
	x += CARD_W;
      }
    }

    if(h < game->total_player_hands - 1)
    {
      x += HAND_DRAW_SPACING;
    }
  }  
}

bool need_to_shuffle(const struct Game *game)
{
  unsigned used = (unsigned)((game->shoe.current_card / (double) game->shoe.num_cards) * 100.0);

  for(unsigned x = 0; x < MAX_DECKS; ++x)
  {
    if(game->num_decks == game->shuffle_specs[x][1] && used > game->shuffle_specs[x][0])
    {
      return true;
    }
  }
  
  return false;
}

void swap(struct Card *a, struct Card *b)
{
  struct Card tmp = *a;
  *a = *b;
  *b = tmp;
}

void shuffle(struct Shoe *shoe)
{
  for(unsigned x = 0; x < 7; x++)
  {
    for(unsigned i = shoe->num_cards - 1; i > 0; i--)
    {
      swap(&shoe->cards[i], &shoe->cards[myrand(0, shoe->num_cards - 1)]);
    }
  }

  shoe->current_card = 0;
}

unsigned myrand(unsigned min, unsigned max)
{
    double scaled = rand() / (RAND_MAX + 1.0);
    return (unsigned)((max - min + 1) * scaled + min);
}

bool dealer_upcard_is_ace(const struct DealerHand *dealer_hand)
{
  return is_ace(&dealer_hand->hand.cards[0]);
}

void insure_hand(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  player_hand->bet /= 2;
  player_hand->played = true;
  player_hand->payed = true;
  player_hand->status = Lost;
  game->money -= player_hand->bet;
  
  game->current_menu = MenuGame;
}

void no_insurance(struct Game *game)
{
  struct DealerHand *dealer_hand = &game->dealer_hand;
  struct PlayerHand *player_hand;

  if(is_blackjack(&dealer_hand->hand))
  {
    dealer_hand->hide_down_card = false;
    dealer_hand->played = true;

    pay_hands(game);
    game->current_menu = MenuGame;
    return;
  }
  
  player_hand = &game->player_hands[game->current_player_hand];

  if(player_is_done(game, player_hand))
  {
    play_dealer_hand(game);
    game->current_menu = MenuGame;
    return;
  }
  
  game->current_menu = MenuHand;
}

void ask_insurance(struct Game *game)
{
  printf(" Insurance?  (Y) Yes  (N) No\n");
  bool br = false;
  char c = { 0 };

  while(true)
  {
    c = (char)getchar();

    switch(c)
    {
    case 'y':
      br = true;
      insure_hand(game);
      break;
    case 'n':
      br = true;
      no_insurance(game);
      break;
    default:
      br = true;
      ask_insurance(game);
      break;
    }

    if(br) break;
  }
}

void deal_new_hand(struct Game *game)
{
  struct PlayerHand player_hand = { .bet=game->current_bet };
  struct DealerHand *dealer_hand = &game->dealer_hand;
  struct Shoe *shoe = &game->shoe;

  if(need_to_shuffle(game))
  {
    shuffle(shoe);
  }

  dealer_hand->hide_down_card = true;
  dealer_hand->hand.num_cards = 0;

  deal_card(shoe, &player_hand.hand);
  deal_card(shoe, &dealer_hand->hand);
  deal_card(shoe, &player_hand.hand);
  deal_card(shoe, &dealer_hand->hand);

  game->player_hands[0] = player_hand;
  game->current_player_hand = 0;
  game->total_player_hands = 1;
  
  if(dealer_upcard_is_ace(dealer_hand) && !is_blackjack(&player_hand.hand))
  {
    // TODO
    //ask_insurance(game);
    game->current_menu = MenuHand;
    return;
  }

  if(player_is_done(game, &player_hand))
  {
    dealer_hand->hide_down_card = false;    
    pay_hands(game);
    game->current_menu = MenuGame;
    return;
  }

  game->current_menu = MenuHand;
  save_game(game);
}

void get_new_bet(struct Game *game)
{
  unsigned tmp;

  printf("  Current Bet: $%u  Enter New Bet: $", (game->current_bet / 100));

  if(1 != scanf("%u", &tmp))
  {
    tmp = MIN_BET / 100;
  }

  game->current_bet = tmp * 100;
  normalize_bet(game);
  deal_new_hand(game);
}

void get_new_num_decks(struct Game *game)
{
  unsigned tmp;

  printf("  Number Of Decks: %u  Enter New Number Of Decks (1-8): ", (game->num_decks));

  if(1 != scanf("%u", &tmp))
  {
    tmp = 1;
  }

  if(tmp < 1) tmp = 1;
  if(tmp > 8) tmp = 8;

  game->num_decks = tmp;
  game_options(game);
}

void get_new_deck_type(struct Game *game)
{
  bool br = false;
  char c = { 0 };

  printf(" (1) Regular  (2) Aces  (3) Jacks  (4) Aces & Jacks  (5) Sevens  (6) Eights\n");

  while(true)
  {
    c = (char)getchar();

    switch(c)
    {
    case '1':
      br = true;
      new_regular(game);
      break;
    case '2':
      br = true;
      new_aces(game);
      break;
    case '3':
      br = true;
      new_jacks(game);
      break;
    case '4':
      br = true;
      new_aces_jacks(game);
      break;
    case '5':
      br = true;
      new_sevens(game);
      break;
    case '6':
      br = true;
      new_eights(game);
      break;
    default:
      br = true;
      game_options(game);
    }

    if(br)
    {
      game->current_menu = MenuGame;
      break;
    }
  }
}

void game_options(struct Game *game)
{
  bool br = false;
  char c = { 0 };

  printf(" (N) Number of Decks  (T) Deck Type  (B) Back\n");

  while(true)
  {
    c = (char)getchar();

    switch(c)
    {
    case 'n':
      br = true;
      get_new_num_decks(game);
      break;
    case 't':
      br = true;
      get_new_deck_type(game);
      break;
    case 'b':
      br = true;
      game->current_menu = MenuGame;
      break;
    default:
      br = true;
      game_options(game);
    }

    if(br) break;
  }
}

void process(struct Game *game)
{
  if(more_hands_to_play(game))
  {
    play_more_hands(game);
    return;
  }

  play_dealer_hand(game);
  game->current_menu = MenuGame;
}

void play_more_hands(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[++(game->current_player_hand)];
  deal_card(&game->shoe, &player_hand->hand);

  if(player_is_done(game, player_hand))
  {
    process(game);
    return;
  }

  game->current_menu = MenuHand;
}

void player_hit(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  if(!player_can_hit(player_hand))
    return;

  deal_card(&game->shoe, &player_hand->hand);

  if(player_is_done(game, player_hand))
  {
    process(game);
    return;
  }
}

void player_stand(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  player_hand->stood = true;
  player_hand->played = true;

  if(more_hands_to_play(game))
  {
    play_more_hands(game);
    return;
  }

  play_dealer_hand(game);
  game->current_menu = MenuGame;
}

void player_split(struct Game *game)
{
  struct PlayerHand new_hand = { .bet=game->current_bet };
  unsigned hand_count = game->total_player_hands;
  struct PlayerHand *this_hand;
  struct PlayerHand *split_hand;
  struct Card card;

  if(!player_can_split(game))
  {
    game->current_menu = MenuHand;
    return;
  }

  game->player_hands[game->total_player_hands++] = new_hand;

  while(hand_count > game->current_player_hand)
  {
    game->player_hands[hand_count] = game->player_hands[hand_count - 1];
    --hand_count;
  }

  this_hand = &game->player_hands[game->current_player_hand];
  split_hand = &game->player_hands[game->current_player_hand + 1];

  card = this_hand->hand.cards[1];
  split_hand->hand.cards[0] = card;
  split_hand->hand.num_cards = 1;
  this_hand->hand.num_cards = 1;
  deal_card(&game->shoe, &this_hand->hand);

  if(player_is_done(game, this_hand))
  {
    process(game);
    return;
  }

  game->current_menu = MenuHand;
}

void player_dbl(struct Game *game)
{
  struct PlayerHand *player_hand = &game->player_hands[game->current_player_hand];

  deal_card(&game->shoe, &player_hand->hand);
  player_hand->played = true;
  player_hand->bet *= 2;

  if(player_is_done(game, player_hand))
  {
    process(game);
  }
}

const char *card_to_string(const struct Game *game, const struct Card *card)
{
  return game->card_faces[card->value][card->suit];
}

void new_regular(struct Game *game)
{
  struct Card c;
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c.suit = suit;

      for(unsigned value = 0; value < 13; ++value)
      {
	c.value = value;
	game->shoe.cards[game->shoe.num_cards++] = c;
      }
    }
  }

  shuffle(&game->shoe);
}

void new_aces(struct Game *game)
{
  struct Card c = { .value = 0 };
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks * 5; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c;
    }
  }
}

void new_aces_jacks(struct Game *game)
{
  struct Card c1 = { .value = 0 };
  struct Card c2 = { .value = 10 };
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks * 4; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c1.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c1;

      c2.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c2;
    }
  }

  shuffle(&game->shoe);
}

void new_jacks(struct Game *game)
{
  struct Card c = { .value = 10 };
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks * 5; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c;
    }
  }
}

void new_sevens(struct Game *game)
{
  struct Card c = { .value = 6 };
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks * 5; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c;
    }
  }
}

void new_eights(struct Game *game)
{
  struct Card c = { .value = 7 };
  game->shoe.num_cards = 0;

  for(unsigned deck = 0; deck < game->num_decks * 5; ++deck)
  {
    for(unsigned suit = 0; suit < 4; ++suit)
    {
      c.suit = suit;
      game->shoe.cards[game->shoe.num_cards++] = c;
    }
  }
}
