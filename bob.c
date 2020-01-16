#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include "struct.h"
#include "bobhdr.h"

//Commented in bobhdr.h
Select con = {{0}, {0}, {0}, {'D', 'H', 'S', 'C'}, {'N', 'H', 'P', 'G', 'I'}};

int main(int argc, char const *argv[]) {
    Player *p = malloc(sizeof(Player));
    char playedCard[p->numOfPlayer][3];
    bool initialCheck;
    bool newRound;
    initial_arguments_check(p, argc, argv); /*send acknowledge message*/
    initialCheck = true;
    send_to_hub(p, initialCheck, playedCard);
    initialCheck = false;
    newRound = false;
    allocate(p);
    con.trackMessage = con.inFormat[4]; /*initialize message tracker*/
    do {
        init();
        read_input(p);
        if(p->input[0] == con.inFormat[1]) { /*hand*/
            just_wait();
            check_who_is_playing_now(p);
        } else if(p->input[0] == con.inFormat[0]) { /*NEWROUNDlead*/
            check_who_is_playing_now(p);
            if(p->lead) { /*i am lead player*/
                if(p->cardInHand != 0) {
                    bob_move(p);
                    send_to_hub(p, initialCheck, playedCard);
                }
            } else if(!p->lead) { /*not lead player*/
                just_wait();
            }
            newRound = true;
        } else if(p->input[0] == con.inFormat[2]) { /*PLAYEDturn,SR message*/
            check_who_is_playing_now(p);
            if(my_turn(p)) { /*my turn*/
                if(p->cardInHand != 0) {
                    bob_move(p);
                    send_to_hub(p, initialCheck, playedCard);
                }
            } else if(!my_turn(p)) { /*not my turn*/
                just_wait();
            }
            playedCard[p->currentTurn][0] = p->played[0];
            playedCard[p->currentTurn][1] = p->played[1];
        }
        if((p->currentTurn == p->numOfPlayer - 1) && (newRound == true)) {
            show_all_card_played(p, playedCard);
            newRound = false;
        }
    } while(con.inNow != con.inFormat[3]);
    free_all(p);
    return 0;
}
