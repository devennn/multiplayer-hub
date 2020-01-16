#ifndef ALICEHDR_H
#define ALICEHDR_H

#include <stdbool.h>
#include "struct.h"

typedef struct {
    //Used to store how many cards player for every suit
    //will update new number every round
    int numSuit[4];

    //Used to store rank to be played based on player strategy evry round
    //will update new rank every round
    int rank[4];

    //Used to store index of card chosen. This will be used as index for
    //card suit and rank combination
    int chosen[4];

    //List of card suit
    char cardSuit[4];

    //store first char of message. N for NEWROUND, P for PLAYED,
    //H for HAND, G for gameover and I for Initialize
    //I is to initalize the char not the message format
    char inFormat[5];

    //Store the first char of message. Used to track current message
    char inNow;

    //Store the first char of message. Used to track message before
    char trackMessage;

} Select;
extern Select con;

void initial_arguments_check(Player *p, int argc, char const *argv[]);

void is_number(const char *argv, int errorNum);

void alice_error(int a);

bool my_turn(Player *p);

void init(void);

void allocate(Player *p);

void read_input(Player *p);

void valid_message(Player *p);

void check_in(Player *p);

void newround_message_format(Player *p);

void hand_message_format(Player *p);

void played_message_format(Player *p);

void check_input_card(char *played);

void process_input(Player *p);

void alice_move(Player *p);

void check_alice_suit(Player *p);

void check_alice_rank(Player *p, char *card, int rank, int pos);

int lead_suit_pos(Player *p);

bool check_lead_suit(Player *p);

int decide(Player *p);

void combine(Player *p);

void replace(Player *p);

void check_who_is_playing_now(Player *p);

void send_to_hub(Player *p, bool initialCheck, char playedCard[][3]);

void just_wait(void);

void show_all_card_played(Player *p, char playedCard[][3]);

void free_all(Player *p);

#endif
