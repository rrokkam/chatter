#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_NAME_LEN 16
#define MAX_MESSAGE_LEN (256 - MAX_NAME_LEN - sizeof(keyword_t))

typedef enum {
	LGIN,  // log in
	EUSR,  // invalid user provided
	LIST,  // list online users
	SEND,  // send message
	RECV,  // receive message
	LGOT   // log out
} keyword_t;

void send_message(int sockfd, keyword_t keyword, char *name, char *message);
void recv_message(int sockfd, keyword_t *keyword, char **name, char **message);

#endif
