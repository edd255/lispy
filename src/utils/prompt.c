#include "utils/prompt.h"

#include <gnu/libc-version.h>

#include "common.h"

/// @brief Returns the truncated git hash
/// @return The current truncated git hash
char* get_git_hash(void);

/// @brief Returns the git branch name
/// @return The current git branch name
char* get_git_branch_name(void);

char* get_git_branch_name(void) {
    char* branch_name = malloc(sizeof(char) * BUFSIZE);
    FILE* cmd_output = popen("git rev-parse --abbrev-ref HEAD", "r");
    if (NULL == cmd_output) {
        free(branch_name);
        perror("popen");
        return NULL;
    }
    if (NULL != fgets(branch_name, sizeof(branch_name), cmd_output)) {
        size_t len = strlen(branch_name);
        if (0 < len && '\n' == branch_name[len - 1]) {
            branch_name[len - 1] = '\0';
        }
        return branch_name;
    }
    free(branch_name);
    pclose(cmd_output);
    char* empty = malloc(2 * sizeof(char));
    empty[0] = ' ';
    empty[1] = ' ';
    return empty;
}

char* get_git_hash(void) {
    char* git_hash = malloc(sizeof(char) * BUFSIZE);
    FILE* cmd_output = popen("git rev-parse HEAD", "r");
    if (NULL == cmd_output) {
        free(git_hash);
        perror("popen");
        return NULL;
    }
    if (NULL != fgets(git_hash, sizeof(git_hash), cmd_output)) {
        size_t len = strlen(git_hash);
        if (0 < len && '\n' == git_hash[len - 1]) {
            git_hash[len - 1] = '\0';
        }
        if (len >= 7) {
            git_hash[7] = '\0';
        }
        return git_hash;
    }
    free(git_hash);
    char* empty = malloc(2 * sizeof(char));
    empty[0] = ' ';
    empty[1] = ' ';
    pclose(cmd_output);
    return empty;
}

void print_prompt(void) {
    char* branch_name = get_git_branch_name();
    char* git_hash = get_git_hash();
#if defined(__GNU__)
    printf(
        "Lispy %g (%s %s, %s %s) [GCC %d.%d.%d, libc %s, C%lu]\n",
        VERSION,
        branch_name,
        git_hash,
        __DATE__,
        __TIME__,
        __GNUC__,
        __GNUC_MINOR__,
        __GNUC_PATCHLEVEL__,
        gnu_get_libc_version(),
        __STDC_VERSION__
    );
#elif defined(__clang__)
    printf(
        "Lispy %g (%s %s, %s %s) [clang %d.%d.%d, libc %s, C%lu]\n",
        VERSION,
        branch_name,
        git_hash,
        __DATE__,
        __TIME__,
        __clang_major__,
        __clang_minor__,
        __clang_patchlevel__,
        gnu_get_libc_version(),
        __STDC_VERSION__
    );
#else
    printf(
        "Lispy %g (%s %s, %s %s)\n",
        VERSION,
        branch_name,
        git_hash,
        __DATE__,
        __TIME__
    );
#endif
    printf("Press Ctrl+c or type 'exit' to exit.\n");
    FREE(branch_name);
    FREE(git_hash);
}
