#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

#include "struct.h"
#include "hubhdr.h"

int main(int argc, char const *argv[]) {

    Game *g = malloc(sizeof(Game));
    Hub *h = malloc(sizeof(Hub));
    Process **pr;
    int index;

    initial_arguments_check(argc, argv, h);
    pr = (Process **)malloc(sizeof(Process *) * h->numOfPlayer);
    for(index = 0; index < h->numOfPlayer; index++) {
        pr[index] = (Process *)malloc(sizeof(Player) * h->numOfPlayer);
    }
    allocate_memory(g, h, argc);
    player_arg(g, argc, argv);
    load_deck(g, h, argv[1]);
    start_player_program(pr, h, g);
    send_card(h, pr);

    //game play process
    do {
        initialize_checking_components(h);
        arrange_turns(h);
        h->newroundMes = false;
        send_message(h, pr);
        do {
            read_message(h, pr);
            check_correct_message(h);
            process_received_message(g, h);
            h->playedMes = false;
            send_message(h, pr);
            h->roundCounter++;
        } while(!end_of_round(h));
        display_output(h);
        determine_winner(h);
        score_counter(h, h->roundWinner);
    } while(h->cardGiven != 0);

    //end game stuff
    h->endGame = true;
    score_counter(h, h->roundWinner);
    send_message(h, pr);
    display_output(h);
    kill_process(pr, h);
    free_allocated_memory(g, h, pr, argc);

    return 0;
}
