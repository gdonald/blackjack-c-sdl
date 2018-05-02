
#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"

#define CARDS_PER_DECK 52
#define MAX_CARDS_PER_HAND 11
#define MAX_DECKS 8
#define MAX_PLAYER_HANDS 7
#define MIN_BET 500
#define MAX_BET 10000000
#define SAVE_FILE "bj.txt"

#define SCREEN_W  1024
#define SCREEN_H  768

#define DEALER_HAND_Y_OFFSET 80
#define PLAYER_HANDS_Y_OFFSET 450

#define CARD_BMAP_SPACING 1
#define CARD_DRAW_SPACING 19

#define CARD_W 119
#define CARD_H 166

extern const char *argp_program_version;
extern const char *argp_program_bug_address;
extern const char *doc;
extern const char *args_doc;
extern struct argp_option options[];

enum CountMethod { Soft, Hard };
enum HandStatus { Won=1, Lost, Push };

extern const unsigned shuffle_specs[8][2];
extern const char *const card_faces[14][4];

struct arguments
{
  char *args[1];
  unsigned players;
};

struct Card
{
  unsigned value;
  unsigned suit;
};

struct Shoe
{
  struct Card cards[CARDS_PER_DECK * MAX_DECKS];
  unsigned current_card;
  unsigned num_cards;
};

struct Hand
{
  struct Card cards[MAX_CARDS_PER_HAND];
  unsigned num_cards;
};

struct DealerHand
{
  struct Hand hand;
  bool played;
  bool hide_down_card;
};

struct PlayerHand
{
  struct Hand hand;
  unsigned bet;
  bool stood;
  bool played;
  bool payed;
  enum HandStatus status;
};

struct Game
{
  SDL_Renderer *renderer;
  SDL_Texture *cards_texture;
  struct Shoe shoe;
  struct DealerHand dealer_hand;
  struct PlayerHand player_hands[MAX_PLAYER_HANDS];
  unsigned num_players;
  unsigned num_decks;
  unsigned money;
  unsigned current_bet;
  unsigned current_player_hand;
  unsigned total_player_hands;
  const unsigned (*shuffle_specs)[2];
  const char *const (*card_faces)[4];
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

const char *card_to_string(const struct Game *game, const struct Card *card);

bool is_ace(const struct Card *card);
bool is_ten(const struct Card *card);
bool player_is_busted(const struct PlayerHand *player_hand);
bool is_blackjack(const struct Hand *hand);
bool player_can_hit(const struct PlayerHand *player_hand);
bool player_can_stand(const struct PlayerHand *player_hand);
bool player_can_split(const struct Game *game);
bool player_can_dbl(const struct Game *game);
bool player_is_done(struct Game *game, struct PlayerHand *player_hand);
bool more_hands_to_play(const struct Game *game);
bool need_to_play_dealer_hand(const struct Game *game);
bool dealer_is_busted(const struct DealerHand *dealer_hand);
bool dealer_upcard_is_ace(const struct DealerHand *dealer_hand);
bool need_to_shuffle(const struct Game *game);

unsigned player_get_value(const struct PlayerHand *player_hand, enum CountMethod method);
unsigned dealer_get_value(const struct DealerHand *dealer_hand, enum CountMethod method);
unsigned all_bets(const struct Game *game);
unsigned myrand(unsigned min, unsigned max);

void draw_card(SDL_Renderer *renderer, SDL_Texture *cards_texture, const unsigned col, const unsigned row, const unsigned pos_x, const unsigned pos_y);
void normalize_bet(struct Game *game);
void save_game(const struct Game *game);
void load_game(struct Game *game);
void pay_hands(struct Game *game);
void deal_card(struct Shoe *shoe, struct Hand *hand);
void play_dealer_hand(struct Game *game);
void draw_dealer_hand(const struct Game *game);
void draw_player_hand(const struct Game *game, unsigned index);
void draw_hands(const struct Game *game);
void swap(struct Card *a, struct Card *b);
void shuffle(struct Shoe *shoe);
void insure_hand(struct Game *game);
void no_insurance(struct Game *game);
void ask_insurance(struct Game *game);
void deal_new_hand(struct Game *game);
void get_new_bet(struct Game *game);
void bet_options(struct Game *game);
void game_options(struct Game *game);
void get_new_num_decks(struct Game *game);
void get_new_deck_type(struct Game *game);
void process(struct Game *game);
void play_more_hands(struct Game *game);
void player_hit(struct Game *game);
void player_stand(struct Game *game);
void player_split(struct Game *game);
void player_dbl(struct Game *game);
void player_get_action(struct Game *game);
void new_regular(struct Game *game);
void new_aces(struct Game *game);
void new_aces_jacks(struct Game *game);
void new_jacks(struct Game *game);
void new_sevens(struct Game *game);
void new_eights(struct Game *game);

void handle_click(struct Game *game, SDL_MouseButtonEvent *button, int mouse_x, int mouse_y);

#endif
