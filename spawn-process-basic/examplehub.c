#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>

typedef struct {
    int playerPid;
    FILE* getFromChild;
    FILE* sendToChild;
} Player ;

typedef struct {
    int numberOfPlayers;
} Game ;

int spawn_child(Game *game, Player **player);

void start_process(Game *game, Player **player, char *usedargv[], int playerId);

void read_input(Player **player, int playerId);

void send_message(Player **player, int playerId);

int main(int argc, char const *argv[]) {

    Game *game = malloc(sizeof(Game));
    Player **player;

    scanf("%d", &game->numberOfPlayers);

    player = (Player **)malloc(sizeof(Player *) * game->numberOfPlayers);
    for(int i = 0; i < game->numberOfPlayers; i++) {
        player[i] = (Player *)malloc(sizeof(Player) * game->numberOfPlayers);
    }

    spawn_child(game, player);

    for(int i = 0; i < game->numberOfPlayers; i++) {
        send_message(player, i);
        read_input(player, i);
    }

    return 0;
}

int spawn_child(Game *game, Player **player)
{
    char* usedargv[] = {NULL, NULL};

    for(int playerId = 0; playerId < game->numberOfPlayers; playerId++) {
        usedargv[0] = "./b";
        start_process(game, player, usedargv, playerId);
        printf("Done Spawn %d\n", playerId);
    }

    return 0;
}

void start_process(Game *game, Player **player, char *usedargv[], int playerId)
{
    int toChild[2], fromChild[2];
    char buffer = 0;

    pipe(toChild);
    pipe(fromChild);

    player[playerId]->playerPid = fork();

    if(player[playerId]->playerPid) { //parent
        player[playerId]->sendToChild = fdopen(toChild[1], "w");
        player[playerId]->getFromChild = fdopen(fromChild[0], "r");
    } else { //child
        dup2(toChild[0], STDIN_FILENO);
        dup2(fromChild[1], STDOUT_FILENO);
        execvp(usedargv[0], usedargv);
    }

}

void read_input(Player **player, int playerId)
{
    FILE* receivesMes = player[playerId]->getFromChild;

    char *line;
    line = (char*)malloc(100);

    fprintf(stderr, "[WAITING FOR INPUT]\n");
    fgets(line, 100, receivesMes);
    fprintf(stderr, "%s", line);

}

void send_message(Player **player, int playerId)
{
    FILE* sendThisMes = player[playerId]->sendToChild;

    fprintf(stderr, "[Attempt sending]\n");
    fprintf(sendThisMes, "%d\n", playerId);
    fflush(sendThisMes);
}
