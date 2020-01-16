#ifndef HUBHDR_H
#define HUBHDR_H

#include <stdbool.h>
#include "struct.h"

//Comment for functions usage are in hubsrc.c

void initial_arguments_check(int argc, char const *argv[], Hub *h);

bool check_suit(char x);

void allocate_memory(Game *g, Hub *h, int argc);

void exit_program(int a);

void load_deck(Game *g, Hub *h, const char *deckFile);

void valid_card(Game *g);

void player_arg(Game *g, int count, char const *playerProgram[]);

bool check_format(const char *playerProgram);

void split_card(Game *g, Hub *h);

void read_message(Hub *h, Process **pr);

void check_correct_message(Hub *h);

void process_received_message(Game *g, Hub *h);

void check_player_hand(Game *g, Hub *h);

void initialize_checking_components(Hub *h);

void send_card(Hub *h, Process **pr);

void send_message(Hub *h, Process **pr);

bool end_of_round(Hub *h);

void determine_winner(Hub *h);

void score_counter(Hub *h, int winPlayer);

void display_output(Hub *h);

int start_player_program(Process **pr, Hub *h, Game *g);

void start_process(Game *g, Process **pr, char *usedargv[], int playerId);

bool read_acknowledge(Process **pr, Hub *h);

void arrange_turns(Hub *h);

void free_allocated_memory(Game *g, Hub *h, Process **pr, int argc);

void kill_process(Process **pr, Hub *h);

#endif
