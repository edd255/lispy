#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>

int main() {
        printf("CLisp 0.1\n");
        printf("Press Ctrcl+c to exit.\n");

        while (true) {
                char* input = readline("> ");
                (void) add_history(input);
                free(input);
        }

        return 0;
}
