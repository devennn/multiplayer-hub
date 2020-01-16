#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include "struct.h"
#include "bobhdr.h"

/*Initial check for number of arguments, number of player and player position
*Take argument of argument count, program argument and struct Player
*/
void initial_arguments_check(Player *p, int argc, char const *argv[])
{
    //chek argument count
    if(argc != 5) {
        bob_error(1);
    }
    //chek num of player
    is_number(argv[1], 2);
    if(atoi(argv[1]) < 2) {
        bob_error(2);
    }
    p->numOfPlayer = atoi(argv[1]);
    //check position
    is_number(argv[2], 3);
    p->position = atoi(argv[2]);
    if(atoi(argv[3]) < 2) {
        bob_error(4);
    }
    //check hand size
    is_number(argv[4], 5);
    if(atoi(argv[4]) < 1) {
        bob_error(5);
    } else {
        p->pHand = atoi(argv[4]);
        p->cardInHand = p->pHand;
    }
}

/*check if string is number. Take argument of argv string and error number
*that will be triggered if not following format
*/
void is_number(const char *str, int errorNum)
{
    int num;

    for(num = 0; num < strlen(str); num++) {
        if(!isdigit(str[num])) {
            bob_error(errorNum);
        }
    }
}

/*Print error message and exit program. Take argument of status number
*and trigger error according to error status
*/
void bob_error(int a)
{
    switch(a) {
        case 1:
            fprintf(stderr, "Usage: player players myid threshold handsize\n");
            exit(a);
            break;

        case 2:
            fprintf(stderr, "Invalid players\n");
            exit(a);
            break;

        case 3:
            fprintf(stderr, "Invalid position\n");
            exit(a);
            break;

        case 4:
            fprintf(stderr, "Invalid threshold\n");
            exit(a);
            break;

        case 5:
            fprintf(stderr, "Invalid hand size\n");
            exit(a);
            break;

        case 6:
            fprintf(stderr, "Invalid message\n");
            exit(a);
            break;

        case 7:
            fprintf(stderr, "EOF\n");
            exit(a);
            break;
    }
}

/*Decide player turn to play. Take argument of struct Player. Will return
*trun if it is player turn to play and false if not. For safety, player
*if there is no card left.
*/
bool my_turn(Player *p)
{
    if((p->leadPlayer == p->numOfPlayer - 1) && (p->position == 0)) {
        //if round winner is last player and i am index 0
        if(((p->position + p->leadPlayer) == p->currentTurn) &&
                (p->cardInHand != 0)) {
            return true;
        }
    } else {
        //every other move
        if(((p->position - 1) == p->currentTurn) && (p->cardInHand != 0)) {
            return true;
        } else if((p->position == 0) && (p->cardInHand != 0)) {
            return true;
        }
    }

    return false;
}

/*Init all card chose by player. Use global variable specified in Main program
*in bob.c
*/
void init(void)
{
    int index;
    for(index = 0; index < 4; index++) {
        con.numSuit[index] = 0;
        con.rank[index] = 0;
    }
    con.chosen[0] = 0;
    con.chosen[1] = 0;
}

/*Allocating memory for variables. Take argument of struct Game,
*Hub and argument count
*/
void allocate(Player *p)
{
    int index;

    p->hand = (char **)malloc(p->pHand * sizeof(char *));
    for(index = 0; index < p->pHand; index++) {
        p->hand[index] = (char *)malloc(4 * sizeof(char));
    }

    p->move = (char *)malloc(4 * sizeof(char));
    p->played = (char *)malloc(4 * sizeof(char));
    p->playNow = (char *)malloc(10 * sizeof(char));
    p->input = (char *)malloc(100 * sizeof(char));
}

/*Read input from stdin. Take argument of struct Player. Will check if input
*is available. If not, trigger error
*/
void read_input(Player *p)
{
    if(!fgets(p->input, 100, stdin)) {
        bob_error(7);
    }
    check_in(p);
}

/*Check if message is in correct order. Take argument af struct Player.
*Wll trigger error if message is incorrect and exit program
*/
void valid_message(Player *p)
{
    int before = (int)con.trackMessage;
    int now = (int)p->input[0];
    if(p->input[0] == con.inFormat[0]) {
        /*newwround valid message*/
        if(((now - before) != 6) && ((before - now) != 2)) {
            /*must be hand or played*/
            bob_error(6);
        }
    } else if(p->input[0] == con.inFormat[1]) {
        /*hand valid message*/
        if((before - now) != 1) {
            bob_error(6);
        }
    } else if(p->input[0] == con.inFormat[2]) {
        /*played valid message*/
        if((now - before) != 2 && (now - before) != 0) {
            bob_error(6);
        }
    }

}

/*Check hand input and harvest information needed to run. Take argument of
*struct Player.
*/
void check_in(Player *p)
{
    if(p->input[0] == con.inFormat[0]) { /*newround*/
        newround_message_format(p);
    } else if(p->input[0] == con.inFormat[1]) { /*hand*/
        hand_message_format(p);
    } else if(p->input[0] == con.inFormat[2]) { /*played*/
        played_message_format(p);
    } else if(p->input[0] == con.inFormat[3]) { /*gameover*/
        con.inNow = con.inFormat[3];
    } else {
        bob_error(6);
    }
}

/*Harvest NEWROUDND message such as lead player. Will also determine
*if player is the leader. Take argument of struct Player and use global
*variable for message tracking
*/
void newround_message_format(Player *p)
{
    con.inNow = con.inFormat[1];
    valid_message(p);
    char *buff = malloc(strlen(p->input) * sizeof(char));
    strcpy(buff, p->input);
    p->leadPlayer = buff[strlen(buff) - 2] - '0';
    if(p->leadPlayer > p->numOfPlayer - 1) {
        bob_error(6);
    }
    p->lead = (p->leadPlayer == p->position) ? true : false;
    con.trackMessage = p->input[0];
}

/*Harvest HAND message such as handsize and player card.
*Will also determine if player is the leader. Take argument of struct Player
*and use global variable for message tracking
*/
void hand_message_format(Player *p)
{
    int count = 0;
    int index = 0;
    con.inNow = con.inFormat[0];
    valid_message(p);
    p->format = strtok(p->input, ",");
    while(p->format != NULL) {
        if(count > 0) {
            strcpy(p->hand[index], p->format);
            p->hand[index][strlen(p->hand[index])] = '\0';
            index++;
        }
        p->format = strtok(NULL, ",");
        count++;
    }
    //remove newline
    for(count = 0; count < p->pHand; count++) {
        for(index = 0; index < strlen(p->hand[count]); index++) {
            if(p->hand[count][index] == '\n') {
                p->hand[count][index] = '\0';
            }
        }
    }
    //for message tracking
    con.trackMessage = p->input[0];
}

/*Harvest PLAYED message such who is playing right now and card played.
*Take argument of struct Player. Will record the card played
*and use global variable for message tracking
*/
void played_message_format(Player *p)
{
    int index = 0;

    con.inNow = con.inFormat[2];
    valid_message(p);
    p->format = strtok(p->input, ",");
    while(p->format != NULL) {
        if(index == 0) {
            strcpy(p->playNow, p->format);
            p->playNow[strlen(p->playNow)] = '\0';
            index++;
        }
        if(index > 0) {
            strcpy(p->played, p->format);
            p->played[strlen(p->played)] = '\0';
            index++;
        }
        p->format = strtok(NULL, ",");
        index++;
        process_input(p);
    }
    if((p->playNow[6] - 48) == p->leadPlayer) {
        p->leadSuit = p->played[0]; /*record the lead player suit*/
        p->playedSuit = p->played[0]; /*record the suit played before*/
        p->playedRank = p->played[1];
    } else {
        p->playedSuit = p->played[0]; /*record the suit played before*/
        p->playedRank = p->played[1];
    }
    check_input_card(p->played);
    con.trackMessage = p->input[0]; /*for message tracking*/
}

/*Check input card wether they of the correct format. Take arguments of
*card string. Card must be of suit SDCH and rank of 1 - 9 and a - f.
*If condition not met. will exit program.
*/

/*Check input card wether they of the correct format. Take arguments of
*card string. Card must be of suit SDCH and rank of 1 - 9 and a - f.
*If condition not met. will exit program.
*/
void check_input_card(char *played)
{
    int check;
    bool validSuit = false;

    for(check = 0; check < 4; check++) {
        if(played[0] == con.cardSuit[check]) {
            validSuit = true;
        }
    }
    if(!validSuit) {
        bob_error(6);
    }
    if(!isdigit(played[1]) && !isxdigit(played[1])) {
        bob_error(6);
    }
    if(isupper(played[1])) {
        bob_error(6);
    }
}

/*Remove Newline from input*/
void process_input(Player *p)
{
    int index;
    for(index = 0; index < strlen(p->played); index++) {
        if(p->played[index] == '\n') {
            p->played[index] = '\0';
        }
    }
    for(index = 0; index < strlen(p->played); index++) {
        if(p->playNow[index] == '\n') {
            p->playNow[index] = '\0';
        }
    }
}

/*Determine which move does bob strategy need to use. Take argument of
*struct Player.
*/
void bob_move(Player *p)
{
    if(p->lead) {
        check_bob_suit(p);
        decide(p);
        combine(p);
        p->cardInHand--;
    } else if(!p->lead) {
        if(check_lead_suit(p) == true) {
            check_bob_suit(p);
            decide(p);
        } else {
            check_bob_suit(p);
            decide(p);
        }
        combine(p);
        p->cardInHand--;
    }
}

/*Check Suit. Will be used to update card played. Take argument of
*struct Player.
*/
void check_bob_suit(Player *p)
{
    int index;
    int rank;
    for(index = 0; index < p->pHand; index++) {
        if(p->hand[index][0] == con.cardSuit[0]) {
            con.numSuit[0] += 1;
            rank = p->hand[index][1] - '0';
            check_bob_rank(p, p->hand[index], rank, 0);
        } else if (p->hand[index][0] == con.cardSuit[1]) {
            con.numSuit[1] += 1;
            rank = p->hand[index][1] - '0';
            check_bob_rank(p, p->hand[index], rank, 1);
        } else if (p->hand[index][0] == con.cardSuit[2]) {
            con.numSuit[2] += 1;
            rank = p->hand[index][1] - '0';
            check_bob_rank(p, p->hand[index], rank, 2);
        } else if (p->hand[index][0] == con.cardSuit[3]) {
            con.numSuit[3] += 1;
            rank = p->hand[index][1] - '0';
            check_bob_rank(p, p->hand[index], rank, 3);
        }
    }
}

/*Chek if current card in hand is lead suit. Take struct Player as
*argument. Return true if lead suit and false if not
*/
bool is_it_leadsuit(Player *p, int index)
{
    if(p->hand[index][0] == p->leadSuit) {
        return true;
    } else {
        return false;
    }
}

/*Check if player have lead suit in hand. Take argument of struct Player
*Return trus if there is a leadsuit in players hand and false otherwise
*/
bool check_lead_suit(Player *p)
{
    int index;

    for(index = 0; index < p->pHand; index++) {
        if(p->hand[index][0] == p->leadSuit) {
            return true;
        }
    }
    return false;
}

/*Check rank for bob move. Rank strategy will be different according
*to situation such as lead player, not lead player and have lead suit
*and not lead player but no lead suit. Take argument of struct Player,
*card string current card rank and card position.
*/
void check_bob_rank(Player *p, char *card, int rank, int pos)
{

    if(p->lead) {
        if(con.rank[pos] == 0) {
            //for comparison
            con.rank[pos] = 102;
        }
        // play lowest if lead
        if((rank < con.rank[pos]) && (rank != 0)) {
            con.rank[pos] = rank;
        }
    } else if(!p->lead && check_lead_suit(p)) {
        // play lowest if not lead and have lead suit
        if(con.rank[pos] == 0) {
            con.rank[pos] = rank;
        } else if(rank < con.rank[pos]) {
            con.rank[pos] = rank;
        }
    } else if(!p->lead && !check_lead_suit(p)) {
        //play highest if not lead and have no lead suit
        if(rank > con.rank[pos]) {
            con.rank[pos] = rank;
        }
    }
}

/*Find the lead suit position from the global variables. Take argument
*of struct player and use global variable to check suit
*/
int lead_suit_pos(Player *p)
{
    int suit;
    for(suit = 0; suit < 4; suit++) {
        if(con.cardSuit[suit] == p->leadSuit) {
            return suit;
        }
    }

    return -1;
}

/*Decide which card to play according to srategy. Take argument of struct
*Player. Function will follow the suit checked specified in spec. Funtion
*will exit once a card has been decided
*/
int decide(Player *p)
{
    int suit;
    if(p->lead) {
        for(suit = 0; suit < 4; suit++) {
            if(con.rank[suit] > 0) {
                con.chosen[1] = con.rank[suit];
                con.chosen[0] = suit;
                break;
            }
        }
    } else if(!p->lead && !check_lead_suit(p)) {
        if(con.rank[2] > 0) {
            con.chosen[1] = con.rank[2];
            con.chosen[0] = 2;
            return 0;
        } else if(con.rank[3] > 0) {
            con.chosen[1] = con.rank[3];
            con.chosen[0] = 3;
            return 0;
        } else if(con.rank[0] > 0) {
            con.chosen[1] = con.rank[0];
            con.chosen[0] = 0;
            return 0;
        } else if(con.rank[1] > 0) {
            con.chosen[1] = con.rank[1];
            con.chosen[0] = 1;
            return 0;
        }
    } else if(!p->lead && check_lead_suit(p)) {
        if((con.cardSuit[2] == p->leadSuit) && (con.rank[2] > 0)) {
            con.chosen[1] = con.rank[2];
            con.chosen[0] = 2;
            return 0;
        } else if((con.cardSuit[3] == p->leadSuit) && (con.rank[3] > 0)) {
            con.chosen[1] = con.rank[3];
            con.chosen[0] = 3;
            return 0;
        } else if((con.cardSuit[0] == p->leadSuit) && (con.rank[0] > 0)) {
            con.chosen[1] = con.rank[0];
            con.chosen[0] = 0;
            return 0;
        } else if((con.cardSuit[1] == p->leadSuit) && (con.rank[1] > 0)) {
            con.chosen[1] = con.rank[1];
            con.chosen[0] = 1;
            return 0;
        }
    }
    return 0;
}

/*Combine chosen card into one string. Take arguments of struct Player
*Player hand contain card will be replaced with XX to indicate empty
*/
void combine(Player *p)
{
    p->move[0] = con.cardSuit[con.chosen[0]];
    if(p->lead == true) {
        p->leadSuit = p->move[0];
    }
    p->move[1] = (char)(con.chosen[1] + 48);
    p->move[2] = '\0';
    replace(p);
}

/*Replace card with empty indicated by X. Take argument of struct Player
*/
void replace(Player *p)
{
    char empty = 'X';
    int i;
    for(i = 0; i < p->pHand; i++) {
        if((p->move[0] == p->hand[i][0]) && (p->move[1] == p->hand[i][1])) {
            p->hand[i][0] = empty;
            p->hand[i][1] = empty;
            break;
        }
    }
}

/*Check who is playing now. WIll be used to update currentTurn variables
*every time new message come in. Take arguments of struct Player
*/
void check_who_is_playing_now(Player *p)
{
    if(p->input[0] == con.inFormat[2]) {
        p->currentTurn = p->playNow[strlen(p->playNow) - 1] - '0';
    }
}

/*Send message to Hub. Used initially to send acknowledge message and used
*other place for send card played. Take arguments of struct Player,
*initialcheck and array of played card. This function also record card played
*by the player itself
*/
void send_to_hub(Player *p, bool initialCheck, char playedCard[][3])
{
    if(initialCheck == true) {
        fprintf(stdout, "@");
        fflush(stdout);
    } else {
        fprintf(stdout, "PLAY%s\n", p->move);
        fflush(stdout);
        playedCard[p->position][0] = p->move[0];
        playedCard[p->position][1] = p->move[1];
    }
}

/*Do nothing. Used to wait when player should not move. Take no arguments*/
void just_wait(void)
{

}

/*Show card played at the end of round. Send message to stderr. Take arguments
*of struct Player and array of card played
*/
void show_all_card_played(Player *p, char playedCard[][3])
{
    int player;
    char suit;
    char rank;
    fprintf(stderr, "Lead player=%d: ", p->leadPlayer);
    for(player = 0; player < p->numOfPlayer; player++) {
        suit = playedCard[player][0];
        rank = playedCard[player][1];
        fprintf(stderr, "%c.%c", suit, rank);
        if(player < p->numOfPlayer - 1) {
            fprintf(stderr, " ");
        }
    }
    fprintf(stderr, "\n");
}

/*Free all allocated memmory. Take argument of struct Player
*/
void free_all(Player *p)
{
    int this;

    for(this = 0; this < p->pHand; this++) {
        free(p->hand[this]);
    }
    free(p->hand);
    free(p->move);
    free(p->input);
    free(p->format);
    free(p->played);
    free(p->playNow);
}
