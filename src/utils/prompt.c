#include "utils/prompt.h"

#include <gnu/libc-version.h>

#include "common.h"
#include "utils/git_info.h"

void print_prompt(void) {
#if defined(__GNU__)
    printf(
        "Lispy %g (%s %s, %s %s) [GCC %d.%d.%d, libc %s, C%lu]\n",
        VERSION,
        LISPY_GIT_BRANCH_NAME,
        LISPY_GIT_COMMIT_HASH,
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
        LISPY_GIT_BRANCH_NAME,
        LISPY_GIT_COMMIT_HASH,
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
        LISPY_GIT_BRANCH_NAME,
        LISPY_GIT_COMMIT_HASH,
        __DATE__,
        __TIME__
    );
#endif
    printf("Press Ctrl+c or type 'exit' to exit.\n");
}
