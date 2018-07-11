#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "stdin_parser.h"
#include "debug.h"

void parse_stdin(int server_sockfd) {
    char *save, *buf = NULL;
    size_t size = 0;
    /* ssize_t linelen = */ getline(&buf, &size, stdin);
    buf[strlen(buf) - 1] = '\0';  // remove \n at the end
    debug(10, "Buffer: %s\n", buf);

    if (!strcmp(buf, HELP_COMMAND)) {
        printf(HELP_STRING);
    } else if (!strcmp(buf, LOGOUT_COMMAND)) {
        logout(server_sockfd);
    } else if (!strcmp(buf, LIST_COMMAND)) {
        list(server_sockfd);
    } else if (!strcmp(strtok_r(buf, " ", &save), CHAT_COMMAND)) {
        char *name = strtok_r(NULL, " ", &save);
        char *msg = name + strlen(name) + 1;  // +1 to avoid " " character
        if (name == NULL || msg == NULL) {
            debug(5, "Can't split chat command into name and message.")
            printf(HELP_STRING);
        } else {
            message(server_sockfd, name, msg);
        }
    } else {
        printf(INVAL_STRING);
    }
    free(buf);
}
