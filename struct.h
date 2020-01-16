#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

typedef struct {
    //Store player pid. Used in killing process
    pid_t playerPid;

    //File pointer for storing file descriptor.
    //Enable player to send message to hub
    FILE *getFromPlayer;

    //File pointer for storing file descriptor.
    //Enable hub to send message to pleyer
    FILE *sendToPlayer;

} Process;

typedef struct {
    //Store file content when reading from deck file
    char **fileContent;

    //total content of line read from deckfle
    int ttlContent;

    //store player program
    //Used to start player process
    char **argPlayer;

    //Store format of player program
    //Used to determine ways to execute the player program
    bool *playerFormat;

} Game;

typedef struct {
    //store card rank
    //Used for checking
    char *rank;

    //store card suit
    //Used for checking
    char *suit;

    //Store current input of card rank
    char *inRank;

    //Store current input of card suit
    char *inSuit;

    //player hand size
    int handSize;

    //Number of player playing
    int numOfPlayer;

    //lead player for current round
    int leadPlayer;

    //All cards that has been given to player
    //will decrement everytime a player play cards
    int cardGiven;

    //Thresold value
    int threshold;

    //Store input that is sent by player
    char *input;

    //count the round. Will reset every newround
    int roundCounter;

    //winning player for curent round
    int roundWinner;

    //check if PLAYED message has been sent
    bool playedMes;

    //check is NEWROUND message has been sent
    bool newroundMes;

    //check is it is time to show the score
    bool scoreMes;

    //store every player score. Will update after every round ends
    int *playerScore;

    //Keep track of the lead suit of current round
    char leadSuit;

    //Cound the d Card for the round
    int dCardCounter;

    //Used to store player turns. Start with 0,1,2,3.... for first round
    //Will change depending on who is winning for that round
    int *playerTurns;

    //Keep track of the value of V score of every player
    int *vCounter;

    //Check who's turn is it now
    int turn;

    //Keep track of how many cards received. Used to indicate round ends
    //cards received for every round must be the same as number of players to
    //indicate round ends
    int cardReceived;

    //keep track of PLAYED message sent, to make sure every player get
    //the message before sending NEWROUND message
    int playedMesSent;

    //check for end game
    bool endGame;
} Hub;

typedef struct {
    //to check if player is leading
    bool lead;

    //store card given by hub
    char **hand;

    //store player move
    char *move;

    //keep track of the lead suit of the round
    char leadSuit;

    //keep track the played suit for the round
    char playedSuit;

    //keep track for the played rank for the round
    char playedRank;

    //store the input send by hub
    char *input;

    //store message that has been processed
    char *format;

    //store played card for the round
    char *played;

    //store player number after harvested from message
    char *playNow;

    //number of card left in hand
    int cardInHand;

    //player position
    int position;

    //number of card in hand
    int pHand;

    //store the lead player for the round
    int leadPlayer;

    //store id of player that is playing now
    int currentTurn;

    //store D card suit threshold
    int threshold;

    //number of player playing
    int numOfPlayer;
} Player;

#endif
