#include "utils/logger.h"

#include <stdlib.h>
#include <string.h>

FILE* prepare_logfile(void) {
    FILE* log = NULL;
    char* cache_dir = getenv("XDG_CACHE_HOME");
    if (NULL == cache_dir) {
        char* tmp_dir = "/tmp";
        size_t log_file_size = strlen(tmp_dir) + strlen("/lispy/lispy.log") + 1;
        char* log_file = malloc(log_file_size);
        strlcpy(log_file, tmp_dir, log_file_size);
        strlcat(log_file, "/lispy/lispy.log", log_file_size);
        log = fopen(log_file, "we");
        free(log_file);
        return log;
    }
    size_t log_file_size = strlen(cache_dir) + strlen("/lispy/lispy.log") + 1;
    char* log_file = malloc(log_file_size);
    strlcpy(log_file, cache_dir, log_file_size);
    strlcat(log_file, "/lispy/lispy.log", log_file_size);
    log = fopen(log_file, "we");
    free(log_file);
    return log;
}
