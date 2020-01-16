#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {

    char *in = malloc(100 * sizeof(char));
    fgets(in, 100, stdin);

    fprintf(stdout, "My name is bob %s\n", in);
    fflush(stdout);

    return 0;
}
