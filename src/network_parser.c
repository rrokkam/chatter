#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "network_parser.h" 
#include "debug.h"
#include "wrappers.h"

char *keyword_to_string(keyword_t keyword) {
    return (char *) keyword_string[keyword];
}

int string_to_keyword(char *string) {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (strcmp(keyword_string[i], string) == 0) {
            return i;
        }
    }
    return -1;
}

msg_t *set_message(msg_t *msg, keyword_t keyword, char *name, char *payload) {
    msg->keyword = keyword;
    msg->name = name;
    msg->payload = payload;
    return msg;
}

void free_message(msg_t *ptr) {
    free(ptr->bufptr);
    free(ptr);
}

// Parsing Functions

msg_t *parse_message(char *me2u_msg) {

    // Null terminate the me2u message
    char *term = strstr(me2u_msg, ME2U_TERM);
    if (term == NULL) {
        debug(5, "Did not find me2u terminator in message.")
        exit(EXIT_FAILURE);  // can this even happen?
    }
    *term = '\0';

    // Parse keyword leading the me2u message
    char *saveptr = me2u_msg;
    char *keystring = strtok_r(me2u_msg, " ", &saveptr);

    if (keystring == NULL) {
        debug(2, "Got garbage from server.");
        exit(EXIT_FAILURE);
    }

    keyword_t keyword = string_to_keyword(keystring);
    if (keyword == -1) {
        debug(2, "Keyword not in ME2U protocol.");
        exit(EXIT_FAILURE);
    }

    msg_t *msg = Malloc(sizeof(msg_t));

    // Parse rest of message based on protocol
    char *next = keystring + strlen(keystring) + 1;
    char *name = NULL, *payload = NULL;
    if (keyword == MOTD) { // MOTD <message>
        payload = next;
    } else if (keyword == UTSIL) { // UTSIL <user list>
        payload = next;
    } else if (keyword == EDNE) { // EDNE <name>
        name = next;
    } else if (keyword == FROM) { // FROM <name> <message>
        name = strtok_r(NULL, " ", &saveptr);
        payload = name + strlen(name) + 1;
    } else if (keyword == UOFF) {
        name = next;
    } else if (keyword == OT) {
        name = next;
    }

    msg->keyword = keyword;
    msg->name = name;
    msg->payload = payload;
    msg->bufptr = me2u_msg;
    return msg;
}

msg_t *me2u_recv(int sockfd, char *term) {

    int result = 0, bytes_read = 0, buff_len = BUFF_FACTOR, term_len = strlen(term);
    char *recv_buff = Malloc(buff_len * sizeof(char));
    
    while (bytes_read <= MAX_MSG_LEN) {
        result = Recv(sockfd, recv_buff + bytes_read, 1, 0);

        // Handle EOF
        if (result == -1 && errno == EAGAIN) {
            debug(2, "Socket timeout.\n");
            exit(EXIT_FAILURE);
        } else if (result == 0) {
            debug(2, "Socket closed.\n");
            free(recv_buff);
            exit(EXIT_FAILURE);
        } else if (result == -1) {
           debug(2, "Socket timed out.");
           exit(EXIT_FAILURE);
        }
        
        bytes_read += result;
        
        // Resize buffer if needed.
        if ((bytes_read + 1) == buff_len) {
            buff_len += BUFF_FACTOR;
            recv_buff = Realloc(recv_buff, buff_len);
        }

        recv_buff[bytes_read] = '\0';

        // Look for ME2U_TERM at end of message.
        if (bytes_read >= 4 && !strcmp(recv_buff + bytes_read - term_len, term)) {
            
            // // Shorten buff_len if needed
            // buff_len = bytes_read + 1;
            // recv_buff = Realloc(recv_buff, buff_len);
            
            // Parse into msg_t struct and return
            msg_t *msg = parse_message(recv_buff);

            return msg;
        }
    }

    debug(2, "No ME2U message seen.");
    free(recv_buff);    
    exit(EXIT_FAILURE);
}
