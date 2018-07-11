#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_NAME_LEN 16
#define MAX_MESSAGE_LEN (256 - MAX_NAME_LEN - sizeof(keyword_t))

/* 
 * To add a new message type, add it here and update the utility functions
 * in protocol.c
 */
typedef enum {
	LGIN,  // log in
	MOTD,  // message of the day
	EUSR,  // invalid user provided
	LIST,  // list online users
	SEND,  // send message
	RECV,  // receive message
	LGOT   // log out
} keyword_t;

void init();
void fini();

/* Utility functions in protocol.c */

// bool expect_name(keyword_t keyword);
// bool expect_message(keyword_t keyword);
// void send_with_length(int sockfd, char *buf);
// void recv_with_length(int sockfd, char *buf);

// abort the program if we fail a send or receive?
// shouldn't send name or message longer than MAX_NAME_LEN or MAX_MESSAGE_LEN respectively.
void send_message(int sockfd, keyword_t keyword, char *name, char *message);
void recv_message(int sockfd, keyword_t *keyword, char **name, char **message);

#endif
