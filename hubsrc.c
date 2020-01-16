#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "struct.h"
#include "hubhdr.h"

#define MAX_PLAYERS_CHAR 3

/*Initial check for number of arguments, threshold and deckFile
*Take argument of argument count, and program argument
*/
void initial_arguments_check(int argc, char const *argv[], Hub *h)
{
    //check commandline arguments
    if(argc < 4) {
        exit_program(1);
    }
    //check threshold
    if(atoi(argv[2]) < 2) {
        exit_program(2);
    } else {
        h->threshold = atoi(argv[2]);
    }
    //check file name
    if(access(argv[1], F_OK) == -1) {
        exit_program(3);
    }
    //calculate number of player
    h->numOfPlayer = argc - 3;
}

/*Check if suit is of SCDH. Take argument of char of suit to be checked
*return false if not of correct Suit
*/
bool check_suit(char x)
{

    if((char)x == 'S') {
        return true;
    } else if((char)x == 'C') {
        return true;
    } else if((char)x == 'D') {
        return true;
    } else if((char)x == 'H') {
        return true;
    } else {
        return false;
    }
}

/*Allocating memory for variables. Take argument of struct Game,
*Hub and argument count
*/
void allocate_memory(Game *g, Hub *h, int argc)
{
    int index;

    h->playerTurns = (int *)malloc(h->numOfPlayer * sizeof(int));
    g->fileContent = malloc(100 * sizeof(char *));
    for(index = 0; index < 100; index++) {
        g->fileContent[index] = malloc(3 * sizeof(char));
    }
    g->argPlayer = (char **)malloc(argc * sizeof(char *));
    for(index = 0; index < argc; index++) {
        g->argPlayer[index] = (char *)malloc(100 * sizeof(char));
    }
    g->playerFormat = (bool *)malloc(argc * sizeof(bool));
    h->input = (char *)malloc(20 * sizeof(char));
    h->inRank = (char *)malloc(g->ttlContent * sizeof(char));
    h->inSuit = (char *)malloc(g->ttlContent * sizeof(char));
    h->playerScore = (int *)malloc(h->numOfPlayer * sizeof(int));
    h->vCounter = (int *)malloc(h->numOfPlayer * sizeof(int));
    h->playerTurns = (int *)malloc(h->numOfPlayer * sizeof(int));
    h->rank = (char *)malloc(10 * sizeof(char));
    h->suit = (char *)malloc(10 * sizeof(char));
}

/*Print error message and exit program. Take argument of status number
*and trigger error according to error status
*/
void exit_program(int status)
{
    char str[] = "Usage: 2310hub deck threshold player0 {player1}";
    switch(status) {
        case 1:
            fprintf(stderr, "%s\n", str);
            exit(status);
            break;

        case 2:
            fprintf(stderr, "Invalid threshold\n");
            exit(status);
            break;

        case 3:
            fprintf(stderr, "Deck error\n");
            exit(status);
            break;

        case 4:
            fprintf(stderr, "Not enough cards\n");
            exit(status);
            break;

        case 5:
            fprintf(stderr, "Player error\n");
            exit(status);
            break;

        case 6:
            fprintf(stderr, "Player EOF\n");
            exit(status);
            break;

        case 7:
            fprintf(stderr, "Invalid message\n");
            exit(status);
            break;

        case 8:
            fprintf(stderr, "Invalid card choice\n");
            exit(status);
            break;

        case 9:
            fprintf(stderr, "Endded due to signal\n");
            exit(status);
            break;
    }
}

/*Load deck file contents. Take arguments of struct Game, Hub and
*deckFile name. This function will exit program if card number is not
*correct or card not enough for player. If enough, hand size will be calculated
*/
void load_deck(Game *g, Hub *h, const char *deckFile)
{
    int index = 0;
    int stringLength;
    FILE *file = fopen(deckFile, "r");

    while(fgets(g->fileContent[index], 5, file)) {
        if(index == 0) {
            stringLength = strlen(g->fileContent[index]);
            g->fileContent[index][stringLength - 1] = '\0';
        } else {
            stringLength = strlen(g->fileContent[index]);
            g->fileContent[index][stringLength - 1] = '\0';
        }
        index++;
    }
    fclose(file);
    g->ttlContent = index;
    valid_card(g);
    if(atoi(g->fileContent[0]) != (index - 1)) {
        exit_program(3);
    }
    split_card(g, h);
    if((g->ttlContent - 1) < h->numOfPlayer) {
        exit_program(6);
    }
    h->handSize = floor((g->ttlContent - 1) / h->numOfPlayer);
}

/*check valid card. Card rank must be 1 - 9, and a - f and following
*suit format. Exit program if condition not satisfied
*/
void valid_card(Game *g)
{
    int index;
    for(index = 1; index < g->ttlContent; index++) {
        if(!check_suit(g->fileContent[index][0]) ||
                !isxdigit(g->fileContent[index][1])) {
            exit_program(3);
        }
        if(isupper(g->fileContent[index][1])) {
            exit_program(3);
        }
    }
}

/*Break player program for. Take argument of struct Game, argument count
*and arrray of arguments.
*/
void player_arg(Game *g, int argc, char const *playerProgram[])
{
    int index = 0;
    int argIndex;
    for(argIndex = 3; argIndex < argc; argIndex++) {
        g->playerFormat[index] = check_format(playerProgram[argIndex]);
        strcpy(g->argPlayer[index], playerProgram[argIndex]);
        index++;
    }
}

/*Check input format. Either using program path or executable file.
*Mark player program as true if format is executable file and
*false if program path. Take argument of player program string
*/
bool check_format(const char *playerProgram)
{
    if((playerProgram[0] == '.') && (playerProgram[1] == '/')) {
        return true;
    } else {
        return false;
    }
}

/*Split rank and suit from file content. Take argument of struct Game
*struct HUB
*/
void split_card(Game *g, Hub *h)
{
    int index = 0;
    int line;
    for(line = 1; line < g->ttlContent; line++) {
        h->rank[index] = g->fileContent[line][1];
        h->suit[index] = g->fileContent[line][0];
        index++;
    }
}

/*Read message from player. Take argument of struct Hub and stuct Process
*Will also check if player is sending message. If pipe is closed, error will
*be triggered
*/
void read_message(Hub *h, Process **pr)
{
    FILE *fromPlayer;
    h->turn = h->playerTurns[h->roundCounter];
    fromPlayer = pr[h->turn]->getFromPlayer;
    if(!fgets(h->input, 10, fromPlayer)) {
        exit_program(6);
    }
}

/*Check message format. Take argument of stuct Hub, Will exit program if
*wrong inFormat
*/
void check_correct_message(Hub *h)
{
    if(h->input[0] != 'P') {
        exit_program(7);
    } else if(h->input[1] != 'L') {
        exit_program(7);
    } else if(h->input[2] != 'A') {
        exit_program(7);
    } else if(h->input[3] != 'Y') {
        exit_program(7);
    }
}

/*Process received message. Take argument of struct Game and Hub. Break intput
*message to rank and suit and check input validity. Any D card played will
*be recorded. cards received will also be counted to make sure every
*player has played
*/
void process_received_message(Game *g, Hub *h)
{
    h->inRank[h->turn] = h->input[strlen(h->input) - 2];
    h->inSuit[h->turn] = h->input[strlen(h->input) - 3];
    if(h->turn == h->roundWinner) {
        h->leadSuit = h->inSuit[h->turn];
    }
    valid_card(g);
    check_player_hand(g, h);
    h->cardReceived += 1;
    if(h->inSuit[h->turn] == 'D') {
        h->dCardCounter += 1;
    }
}

/*Check if cards played is valid from player hands. Take argument of struct
*Game and Hub. Will exit program if unvalid card.
*/
void check_player_hand(Game *g, Hub *h)
{
    int cards;
    bool haveCard = false;
    int r = h->playerTurns[h->turn] * h->handSize;
    int done = 0;
    for(cards = r; cards < g->ttlContent; cards++) {
        if(h->inRank[h->playerTurns[h->turn]] == h->rank[cards]) {
            if(h->inSuit[h->playerTurns[h->turn]] == h->suit[cards]) {
                haveCard = true;
            }
        }
        if(done == (h->handSize - 1)) {
            break;
        }
        done++;
    }
    if(!haveCard) {
        exit_program(8);
    }
}

/*Initialize componetns use for checking. Take arguments of struct Hub*/
void initialize_checking_components(Hub *h)
{
    h->scoreMes = false;
    h->roundCounter = 0;
    h->dCardCounter = 0;
    h->endGame = false;
    h->playedMesSent = 0;
    h->cardReceived = 0;
}

/*Send card to all players. Take argument of struct Hub and struct Process
*Upon success, round winner will be intialize to player 0 because player 0
*start play for first round
*/
void send_card(Hub *h, Process **pr)
{
    FILE *toPlayer;
    int card = 0;
    int give;
    int player;
    int suitIndex, rankIndex;

    for(player = 0; player < h->numOfPlayer; player++) {
        toPlayer = pr[player]->sendToPlayer;
        fprintf(toPlayer, "HAND");
        give = 0;
        while(give < h->handSize) {
            suitIndex = 0;
            rankIndex = 1;
            fprintf(toPlayer, ",%c%c", h->suit[card], h->rank[card]);
            card++;
            give++;
            suitIndex += 2;
            rankIndex += 2;
        }
        fprintf(toPlayer, "\n");
        fflush(toPlayer);
    }
    h->cardGiven = (h->numOfPlayer * h->handSize);
    h->roundWinner = 0;
}

/*Send Message to All Player. Take struct Hub and struct Process as input.
*Output format will be used to trigger which message to send. h->newroundMes
*for NEWROUND, h->playedMesSent for PLAYED message. When gameover Message
*is sent, scoreMessage is activated.
*/
void send_message(Hub *h, Process **pr)
{

    FILE *toPlayer;
    int player;
    int playerTurn = h->turn;

    if(h->newroundMes == false) {
        //NEWROUNDlead
        for(player = 0; player < h->numOfPlayer; player++) {
            toPlayer = pr[player]->sendToPlayer;
            fprintf(toPlayer, "NERWROUND%d\n", h->roundWinner);
            fflush(toPlayer);
        }
        h->newroundMes = true;
    } else if(h->playedMes == false) {
        //PLAYEDturn,SR
        for(player = 0; player < h->numOfPlayer; player++) {
            if(player == playerTurn) {
                continue;
            }
            toPlayer = pr[player]->sendToPlayer;
            fprintf(toPlayer, "PLAYED%d,%c%c\n",
                    playerTurn, h->inSuit[playerTurn], h->inRank[playerTurn]);
            fflush(toPlayer);
            h->playedMesSent += 1;
        }
        h->playedMes = true;
    } else {
        //GAMEOVER
        for(player = 0; player < h->numOfPlayer; player++) {
            toPlayer = pr[player]->sendToPlayer;
            fprintf(toPlayer, "GAMEOVER\n");
            fflush(toPlayer);
        }
        h->scoreMes = true;
    }
}

/*Used to check end of round. Take argument of struct Hub. Round ends is true
*if every player has played and PLAYEDturn,SR message has been sent to
*correct player
*/
bool end_of_round(Hub *h)
{
    if(h->cardReceived != h->numOfPlayer) {
        return false;
    }
    if(h->playedMesSent != ((h->numOfPlayer - 1) * h->numOfPlayer)) {
        return false;
    }
    return true;
}

/*Determine winner for the round. Take struct Hub as argument. Winner is
*determined by highest rank played of the lead suit. If there is no one else
*playing lead suit. The round leader will win
*/
void determine_winner(Hub *h)
{
    int player;
    bool haveWinner = false;
    int lead = h->playerTurns[0];

    for(player = 1; player < h->numOfPlayer; player++) {
        //check for highest card of the same suit
        int check = h->playerTurns[player];
        if((h->inSuit[check] == h->inSuit[lead]) &&
                (h->inRank[check] > h->inRank[lead])) {
            h->roundWinner = h->playerTurns[player];
            haveWinner = true;
        }
    }
    if(haveWinner == false) {
        //if no one is winning,lead player of the round will win:
        //Condition 1 : other player does not have suit or
        //Condition 2 : other player is playing lower rank of lead card suit
        h->roundWinner = h->playerTurns[0];
    }
}

/*Count score for current turns and end of game. Take arguments of
*struct Hub and player that wins for current round. This function will also
*update number of cards left after each round.
*/
void score_counter(Hub *h, int winPlayer)
{

    if(h->endGame == false) {
        //calculatin score for every round
        h->vCounter[winPlayer] += h->dCardCounter;
        h->playerScore[winPlayer] += 1;
    } else {
        //calculate all player's score
        for(int player = 0; player < h->numOfPlayer; player++) {
            if(h->vCounter[player] >= h->threshold) {
                h->playerScore[player] += h->vCounter[player];
            } else {
                h->playerScore[player] -= h->vCounter[player];
            }
        }
    }
    h->cardGiven -= h->numOfPlayer;
}

/*Display Output. Take arguments of struct Hub. Depending on message flag
*activated, the function will display appropriate message.
*/
void display_output(Hub *h)
{
    int player;
    int counter;
    static int printOnce = 0;

    FILE *printTo = stdout;
    if((printOnce == 0) && (h->scoreMes == false)) {
        //for printinf first round
        fprintf(printTo, "Lead player=0\n");
        fprintf(printTo, "Cards=");
        for(counter = 0; counter < h->numOfPlayer; counter++) {
            player = h->playerTurns[counter];
            fprintf(printTo, "%c.%c", h->inSuit[player], h->inRank[player]);
            if(counter < h->numOfPlayer - 1) {
                fprintf(printTo, " ");
            }
        }
    } else if(h->scoreMes == false) {
        //for printing every other round
        fprintf(printTo, "Lead player=%d\n", h->roundWinner);
        fprintf(printTo, "Cards=");
        for(counter = 0; counter < h->numOfPlayer; counter++) {
            player = h->playerTurns[counter];
            fprintf(printTo, "%c.%c", h->inSuit[player], h->inRank[player]);
            if(counter < h->numOfPlayer - 1) {
                fprintf(printTo, " ");
            }
        }
    }
    if(h->scoreMes == true) {
        //only printed at the end of game
        for(counter = 0; counter < h->numOfPlayer; counter++) {
            fprintf(printTo, "%d:%d", counter, h->playerScore[counter]);
            if(counter < h->numOfPlayer - 1) {
                fprintf(printTo, " ");
            }
        }
    }
    printOnce++;
    fprintf(printTo, "\n");
    fflush(printTo);
}

/*Will start player program. Take arguments of strut Process, struct Hub
*and struct Game. If program successfully started, acknowledge will be read
*by all players. Acknowledge message "@" received must be equal to number
*of players in order for successful game initialization
*/
int start_player_program(Process **pr, Hub *h, Game *g)
{
    int playerId;
    char *usedargv[] = {NULL, NULL, NULL, NULL, NULL, NULL};
    char ttlPlayer[MAX_PLAYERS_CHAR];
    char myid[MAX_PLAYERS_CHAR];
    char thr[MAX_PLAYERS_CHAR];
    char hSize[MAX_PLAYERS_CHAR];

    for(playerId = 0; playerId < h->numOfPlayer; playerId++) {

        //assign arguments needed for player execution
        usedargv[0] = g->argPlayer[playerId];
        sprintf(ttlPlayer, "%d", h->numOfPlayer);
        usedargv[1] = ttlPlayer;
        sprintf(myid, "%d", playerId);
        usedargv[2] = myid;
        sprintf(thr, "%d", h->threshold);
        usedargv[3] = thr;
        sprintf(hSize, "%d", h->handSize);
        usedargv[4] = hSize;

        start_process(g, pr, usedargv, playerId);
    }
    if(!read_acknowledge(pr, h)) {
        kill_process(pr, h);
        exit_program(5);
    }
    free(*usedargv);
    return 0;
}

/*Starting player process.Take arguments of struct Game, struct Proces, array
*of strings containing player program and arguments and player id. will return
exit program if error
*/
void start_process(Game *g, Process **pr, char *usedargv[], int playerId)
{
    int toPlayer[2], fromPlayer[2];
    int surpressError;

    pipe(toPlayer);
    pipe(fromPlayer);
    pr[playerId]->playerPid = fork();

    if(pr[playerId]->playerPid) {
        //parent
        close(toPlayer[0]);
        close(fromPlayer[1]);

        pr[playerId]->sendToPlayer = fdopen(toPlayer[1], "w");
        pr[playerId]->getFromPlayer = fdopen(fromPlayer[0], "r");
    } else {
        //child
        close(toPlayer[1]);
        close(fromPlayer[0]);
        surpressError = open("/dev/null", O_WRONLY);
        dup2(toPlayer[0], STDIN_FILENO);
        dup2(fromPlayer[1], STDOUT_FILENO);
        dup2(surpressError, STDERR_FILENO);

        if(g->playerFormat[playerId] == true) {
            execvp(usedargv[0], usedargv);
        } else {
            execv(usedargv[0], usedargv);
        }

        //program is not starting if reach here
        exit_program(5);
    }

}

/*Read acknowledge mesage "@" send by player. Take arguments of struct Process
*and struct Hub. Acknowlede message need to be equal to number of players.
*If not satisfied, return false, else return true
*/
bool read_acknowledge(Process **pr, Hub *h)
{
    int player;
    int playerOk = 0;
    char buff;
    for(player = 0; player < h->numOfPlayer; player++) {
        buff = fgetc(pr[player]->getFromPlayer);
        if(buff == '@') {
            playerOk += 1;
        }
    }
    if(playerOk != (h->numOfPlayer)) {
        //if all player does not send acknowledge, return false
        return false;
    }
    return true;
}

/*Arrange Turns for every round. Take argument of struct Hub. First round
*will always be player 0. Only next round will be based on winning player
*/
void arrange_turns(Hub *h)
{
    int arrange;
    int roundWinner = h->roundWinner;
    int offset = h->numOfPlayer;

    for(arrange = 0; arrange < h->numOfPlayer; arrange++) {
        if(roundWinner < h->numOfPlayer) {
            //if roundwinner is not the last player, arrange as usual
            h->playerTurns[arrange] = roundWinner;
        } else {
            //if roundWinner is last player, need to minus offset
            h->playerTurns[arrange] = roundWinner - offset;
        }
        roundWinner++;
    }
}

/*Free all allocated memmory. Take struct Game, struct Hub, struct
*Process and argument count.
*/
void free_allocated_memory(Game *g, Hub *h, Process **pr, int argc)
{
    int this;

    //struct game
    for(this = 0; this < 100; this++) {
        free(g->fileContent[this]);
    }
    free(g->fileContent);
    for(this = 0; this < argc; this++) {
        free(g->argPlayer[this]);
    }
    free(g->argPlayer);
    free(g->playerFormat);

    //struct hub
    free(h->rank);
    free(h->suit);
    free(h->inRank);
    free(h->inSuit);
    free(h->input);
    free(h->playerScore);
    free(h->playerTurns);
    free(h->vCounter);

    //struct process
    for(this = 0; this < h->numOfPlayer; this++) {
        free(pr[this]);
    }
    free(pr);
    free(g);
    free(h);

}

/* Do killing player process and reaping zombie. Take argument of struct
*Proess and stuct Hub. Pipe associated to process will be flushed before
*terminating process SIGTERM. Will wait for a moment to exit normally.
*if not, force kill by using SIGKILL. Then all zombies will be reclaimed
*/
void kill_process(Process **pr, Hub *h)
{
    int player;
    int killStatus;
    for(player = 0; player < h->numOfPlayer; player++) {
        fflush(pr[player]->sendToPlayer);
        fflush(pr[player]->getFromPlayer);
        //start by terminating
        killStatus = kill(pr[player]->playerPid, SIGTERM);
        sleep(1);
        if(killStatus != 0) {
            //if terminating unsuccessful, kill
            kill(pr[player]->playerPid, SIGKILL);
        }
        fclose(pr[player]->sendToPlayer);
        fclose(pr[player]->getFromPlayer);
        waitpid(pr[player]->playerPid, NULL, WNOHANG);
    }
}
