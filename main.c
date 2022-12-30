#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>

int main() {
        printf("CLisp 0.1");
        printf("Press Ctrcl+c to exit.");

        while (true) {
                char* input = readline("> ");
                add_history(input);
                free(input);
        }

        return 0;
}
