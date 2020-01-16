#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

struct Child {
    pid_t pid;
    FILE *toChild;
    FILE *fromChild;
};

struct Hub {
    struct Child process;
};

struct Game {
    struct Hub players;
};

void read_input(struct Game *game);

int main(int argc, char const *argv[]) {

        struct Game *game;
        game = malloc(sizeof(struct Hub) * 1);

        //A null terminated array of character
        //pointers
        char *args[]={"./EXEC", "1", "2", NULL};
        int receiving[2], sending[2];

        pipe(receiving);
        pipe(sending);

        pid_t id = fork();

        if(id < 0) {
            fprintf(stderr, "Error piping\n");
        } else if(id > 0) {
            //parent
            game->players.process.toChild = fdopen(sending[1], "w");
            game->players.process.fromChild = fdopen(receiving[0], "r");

            read_input(game);

        } else {
            //child
            printf("Here\n");
            dup2(sending[0], STDIN_FILENO);
            dup2(receiving[1], STDOUT_FILENO);

            execvp(args[0],args);

        }

        /*All statements are ignored after execvp() call as this whole
        process(execDemo.c) is replaced by another process (EXEC.c)
        */
        printf("Ending-----");

    return 0;
}

void read_input(struct Game *game)
{
    char *input;
    input = (char*)malloc(100 * sizeof(char));
    FILE *fromChild = game->players.process.fromChild;
    fgets(input, 100, fromChild);
    fprintf(stderr, "%s\n", input);
}
