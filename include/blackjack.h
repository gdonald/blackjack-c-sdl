
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
#include "SDL2/SDL_ttf.h"

#define CARDS_PER_DECK 52
#define MAX_CARDS_PER_HAND 11
#define MAX_DECKS 8
#define MAX_PLAYER_HANDS 6
#define MIN_BET 500
#define MAX_BET 10000000
#define MIN_NUM_DECKS 1
#define MAX_NUM_DECKS 8
#define SAVE_FILE "bj.txt"

#define SCREEN_W  1024
#define SCREEN_H  768

#define DEALER_HAND_Y_OFFSET 50
#define PLAYER_HANDS_Y_OFFSET 440
#define BUTTONS_Y_OFFSET 670

#define CARD_BMAP_SPACING 1
#define CARD_DRAW_SPACING 19
#define HAND_DRAW_SPACING 20

#define CARD_W 119
#define CARD_H 166

#define BTN_W 120
#define BTN_H 40
#define BTN_SPACE 20

#define FONT "font/LiberationSerif-Bold.ttf"

enum CountMethod { Soft, Hard };
enum HandStatus { Won=1, Lost, Push };
enum Buttons
{
  BtnDbl, BtnHit, BtnStand, BtnSplit,
  BtnDeal, BtnBet, BtnOptions, BtnQuit,
  BtnDecks, BtnType, BtnBack,
  BtnRegular, BtnAces, BtnJacks, BtnAcesJacks, BtnSevens, BtnEights,
  BtnInsureYes, BtnInsureNo,
  BtnCount
};
enum ButtonStates { BtnUp=0, BtnDown=40, BtnOff=80 };
enum Menus { MenuGame, MenuHand, MenuNewBet, MenuDecks, MenuNewNumDecks, MenuDeckType, MenuInsurance };
enum FontSizes { FontSm, FontMd, FontLg };

extern const unsigned shuffle_specs[8][2];

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
  unsigned screen_h, screen_w;
  unsigned player_hands_y_offset;
  unsigned player_hands_y_percent;
  unsigned buttons_y_offset;
  unsigned buttons_y_percent;
  struct Shoe shoe;
  struct DealerHand dealer_hand;
  struct PlayerHand player_hands[MAX_PLAYER_HANDS];
  unsigned num_decks;
  unsigned money;
  unsigned current_bet;
  char current_bet_str[8];
  char num_decks_str[4];
  unsigned current_player_hand;
  unsigned total_player_hands;
  const unsigned (*shuffle_specs)[2];
  const char *const (*card_faces)[4];
  SDL_Renderer *renderer;
  SDL_Texture *cards_texture;
  SDL_Texture *btn_textures[BtnCount];
  SDL_Rect btn_rects[BtnCount];
  unsigned current_menu;
  TTF_Font *fonts[3];
};

SDL_Texture *load_cards_texture(SDL_Renderer *renderer);
SDL_Texture *load_bg_texture(SDL_Renderer *renderer);
SDL_Texture *load_rules_texture(SDL_Renderer *renderer);
SDL_Renderer *create_renderer(SDL_Window *window);
SDL_Window *create_window(const struct Game *game);

bool inside_rect(SDL_Rect rect, int x, int y);
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

unsigned hand_width(const unsigned num_cards);
unsigned player_get_value(const struct PlayerHand *player_hand, enum CountMethod method);
unsigned dealer_get_value(const struct DealerHand *dealer_hand, enum CountMethod method);
unsigned all_bets(const struct Game *game);
unsigned myrand(unsigned min, unsigned max);

void handle_new_bet_keystroke(struct Game *game, char *keystroke);
void handle_new_num_decks_keystroke(struct Game *game, char *keystroke);
void draw_bet_menu(struct Game *game);
void load_fonts(struct Game *game);
void draw_bet(const struct Game *game);
void draw_num_decks_menu(struct Game *game);
void draw_money(const struct Game *game);
void write_text(const struct Game *game, const char *text, const int font_size, const int x, const int y);
void draw_card(const struct Game *game, const struct Card *card, const unsigned x, const unsigned y);
void draw_insurance_menu(struct Game *game);
void draw_hand_menu(struct Game *game);
void draw_game_menu(struct Game *game);
void draw_decks_menu(struct Game *game);
void draw_deck_type_menu(struct Game *game);
void draw_menus(struct Game *game);
void load_btn_textures(struct Game *game);
void load_window_icon(SDL_Window *window);

void calculate_offsets(struct Game *game);
void normalize_bet(struct Game *game);
void normalize_num_decks(struct Game *game);
void save_game(const struct Game *game);
void load_game(struct Game *game);
void pay_hands(struct Game *game);
void deal_card(struct Shoe *shoe, struct Hand *hand);
void play_dealer_hand(struct Game *game);
void draw_dealer_hand(const struct Game *game);
void draw_player_hands(const struct Game *game);
void swap(struct Card *a, struct Card *b);
void shuffle(struct Shoe *shoe);
void insure_hand(struct Game *game);
void no_insurance(struct Game *game);
void deal_new_hand(struct Game *game);
void process(struct Game *game);
void play_more_hands(struct Game *game);
void player_hit(struct Game *game);
void player_stand(struct Game *game);
void player_split(struct Game *game);
void player_dbl(struct Game *game);
void new_regular(struct Game *game);
void new_aces(struct Game *game);
void new_aces_jacks(struct Game *game);
void new_jacks(struct Game *game);
void new_sevens(struct Game *game);
void new_eights(struct Game *game);

void handle_click(struct Game *game, SDL_MouseButtonEvent *button, int mouse_x, int mouse_y);

#endif
