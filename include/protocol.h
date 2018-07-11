#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include "network_parser.h"

void from_handler(int sockfd, msg_t *from_msg);

void uoff_handler(msg_t *uoff_msg);

msg_t *recv_handle(int sockfd, int num_expected, ...);

int client_connect(char *server_ip, int server_port);

void login(int sockfd, char *name);

void list(int sockfd);

void message(int sockfd, char *name, char *message);

void logout(int sockfd);

#endif