#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define USAGE_STRING "\
./client [-h] [-d VERBOSITY] NAME SERVER_IP SERVER_PORT                     \n\
-h           Displays this help menu, and returns EXIT_SUCCESS.             \n\
-v VERBOSITY Set the verbosity level of debug info (defaults to 0)          \n\
NAME         This is the username to display when chatting.                 \n\
SERVER_IP    The ip address of the server to connect to.                    \n\
SERVER_PORT   The port to connect to.                                       \n"

int verbosity;

#ifdef DEBUG
#define debug(level, S, ...)                                                  \
    if (verbosity > level) {                                                  \
        fprintf(stderr, "\033[1;35m%s:%s:%d \033[0m" S, __FILE__,             \
                __extension__ __FUNCTION__, __LINE__, ##__VA_ARGS__);         \
        perror(NULL);
    }
#else
#define debug(level, S, ...)
#endif

#endif /* DEBUG_H */
