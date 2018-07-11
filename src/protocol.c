#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "protocol.h"

keyword_t keyword_buf;
char *name_buf;
char *message_buf;

void init() {
	name_buf = malloc(MAX_NAME_LEN);
	message_buf = malloc(MAX_MESSAGE_LEN);
}

void fini() {
	free(name_buf);
	free(message_buf);
}

bool expect_name(keyword_t keyword) {
	switch (keyword) {
		case LGIN: case EUSR: case SEND: case RECV: case LGOT: return true;
		case MOTD: case LIST: return false;
		default: return false;
	}
}

bool expect_message(keyword_t keyword) {
	switch (keyword) {
		case MOTD: case LIST: case SEND: return true;
		case LGIN: case EUSR: case RECV: case LGOT: return false;
		default: return false;
	}
}

void send_with_length(int sockfd, char *buf) {
	int length = strlen(buf);
	send(sockfd, &length, sizeof(int), 0);
	send(sockfd, buf, length, 0);
}

void recv_with_length(int sockfd, char *buf) {
	int length;
	recv(sockfd, &length, sizeof(int), 0);
	recv(sockfd, buf, length, 0); 
}

// need to add handling for when send/recv return an error
// where to do the length checking for arguments?
void send_message(int sockfd, keyword_t keyword, char *name, char *message) {
	send(sockfd, &keyword, sizeof(keyword_t), 0);
	if (expect_name(keyword)) {
		send_with_length(sockfd, name);
	}
	if (expect_message(keyword)) {
		send_with_length(sockfd, message);
	}
}

void recv_message(int sockfd, keyword_t *keyword, char **name, char **message) {
	keyword = &keyword_buf;
	name = &name_buf;
	message = &message_buf;
	recv(sockfd, keyword, sizeof(keyword_t), 0);
	if (expect_name(keyword_buf)) {
		recv_with_length(sockfd, name_buf);
	}
	if (expect_message(keyword_buf)) {
		recv_with_length(sockfd, message_buf);
	}
}

