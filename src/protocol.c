#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "network_parser.h"
#include "debug.h"
#include "wrappers.h"

#define TIMEOUT_S 0
#define TIMEOUT_US 0

bool keyword_expected(keyword_t to_match, keyword_t expected[], int num_expected) {
    for (int i = 0; i < num_expected; i++) {
        if (expected[i] == to_match)
            return true;
    }
    return false;
}

msg_t *recv_handle(int sockfd, int num_expected, ...) {
    va_list ap;

    // Build expected keyword array
    keyword_t expected[num_expected];
    va_start(ap, num_expected);
    for (int i = 0; i < num_expected; i++)
        expected[i] = va_arg(ap, keyword_t);
    va_end(ap);
    
    msg_t *server_msg = NULL;
    do {
        // Recv from server
        if (server_msg != NULL) free_message(server_msg);
        server_msg = me2u_recv(sockfd, ME2U_TERM);
        
        // Handle valid but unexpected responses
        switch (server_msg->keyword) {
            case FROM:
                from_handler(sockfd, server_msg);
                break;
            case UOFF:
                uoff_handler(server_msg);
                break;
            default:
                break; // should have an error here?
        }

    // Loop if response was unexpected.
    } while (!keyword_expected(server_msg->keyword, expected, num_expected));

    return server_msg;
}

void from_handler(int sockfd, msg_t *from_msg) {
    printf("%s: %s\n", from_msg->name, from_msg->payload);

    // ACK From message
    char *to_send = Malloc(strlen(keyword_to_string(MORF)) + 1 + strlen(from_msg->name) + 1);
    sprintf(to_send, "MORF %s", from_msg->name);
    Send(sockfd, to_send, strlen(to_send), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);
    debug("Sent MORF to server.");
}

void uoff_handler(msg_t *uoff_msg) {
    printf("%s has logged off.\n", uoff_msg->name);
}

int client_connect(char *server_ip, int server_port) {
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_S;
    timeout.tv_usec = TIMEOUT_US;

    struct sockaddr_in serv_addr;

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        debug("Socket creation error.");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, 
            sizeof(timeout)) < 0) {
        debug("setsockopt failure.");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        debug("Invalid address.");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        debug("Server refused to connect.");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void login(int sockfd, char *name) {

    // Login SYN
    const char *msg = keyword_to_string(ME2U);
    Send(sockfd, msg, strlen(msg), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);

    // Login ACK
    msg_t *resp_msg = recv_handle(sockfd, 1, U2EM);
    if (resp_msg->keyword != U2EM) {
        debug("Server did not respond with ME2U login ack.");
        free_message(resp_msg);
        exit(EXIT_FAILURE);
    }

    free_message(resp_msg);

    // Login IAM Statement ; IAM <name>
    char *keyword = keyword_to_string(IAM);
    char *iam_msg = Malloc((strlen(keyword) + 1 + strlen(name) + 1) * sizeof(char));
    sprintf(iam_msg, "%s %s", keyword, name);
    Send(sockfd, iam_msg, strlen(iam_msg), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);
    free(iam_msg);

    // Server response
    resp_msg = recv_handle(sockfd, 2, MAI, ETAKEN);
    if (resp_msg->keyword != MAI) {
        if (resp_msg->keyword == ETAKEN) {
            printf("Username is taken.\n");
        } else {
            debug("Unexpected response from server.");
        }
        free_message(resp_msg);
        exit(EXIT_FAILURE);
    }

    free_message(resp_msg);

    // Message of the day.
    resp_msg = recv_handle(sockfd, 1, MOTD);
    if (resp_msg->keyword != MOTD) {
        debug("Server did not respond with message of the day.");
        free_message(resp_msg);
        exit(EXIT_FAILURE);
    }

    printf("%s: %s\n", keyword_to_string(resp_msg->keyword), resp_msg->payload);
    free_message(resp_msg);
}

void list(int sockfd) {
    char *to_send = keyword_to_string(LISTU);
    Send(sockfd, to_send, strlen(to_send), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);

    // Server Response
    msg_t *resp_msg = recv_handle(sockfd, 1, UTSIL);

    // Print users
    if (resp_msg->keyword != UTSIL) {
        debug("Server did not respond with a list of users.");
        free_message(resp_msg);
        exit(EXIT_FAILURE);
    } else {
        char *saveptr, *tok;
        tok = strtok_r(resp_msg->payload, " ", &saveptr);
        while(tok != NULL) {
            printf("%s\n", tok);
            tok = strtok_r(NULL, " ", &saveptr);
        }
        free_message(resp_msg);
    }
}

// What kind of string is name / message going to be here?
void message(int sockfd, char *name, char *message) {

    // Prepare message statement to server.
    char *keyword = keyword_to_string(TO);
    int length = strlen(keyword) + 1 + strlen(name) + 1 + strlen(message) + 1; // TO <name> <message>
    char *to_send = Malloc(length);
    sprintf(to_send, "%s %s %s", keyword, name, message);
    Send(sockfd, to_send, strlen(to_send), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);

    // Server response
    msg_t *resp_msg = recv_handle(sockfd, 2, OT, EDNE);
    if (resp_msg->keyword == EDNE) { // EDNE <name>
        free_message(resp_msg);
        printf("User \"%s\" does not exist.\n", name);
    } else if (resp_msg->keyword == OT) {
        if (resp_msg->name == NULL || strcmp(name, resp_msg->name) != 0) {
            debug("Server returned OT with wrong username" );
            free(resp_msg);
            exit(EXIT_FAILURE);
        }
        free_message(resp_msg);
    }
}

void logout(int sockfd) {

    // Send logout msg to server
    char *to_send = keyword_to_string(BYE);
    Send(sockfd, to_send, strlen(to_send), 0);
    Send(sockfd, ME2U_TERM, TERM_LEN, 0);

    // Server response
    msg_t *resp_msg = recv_handle(sockfd, 1, EYB);

    if (resp_msg->keyword != EYB) {
        debug("Server did not respond with correct logout ack.");
        free_message(resp_msg);
        exit(EXIT_FAILURE);
    }

    free_message(resp_msg);
    exit(EXIT_SUCCESS);
}