#ifndef PROTOCOL_NEW_H
#define PROTOCOL_NEW_H

/*
 * Information about the protocol implemented here is available in PROTOCOL.md.
 */

#define MAX_NAME_LEN 16
#define MAX_MESSAGE_LEN (256 - MAX_NAME_LEN - sizeof keyword_t)

// extern keyword_t keyword_buf;
// extern char *name_buf;
// extern char *message_buf;

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
// void send_keyword(int sockfd, keyword_t keyword);
// void recv_keyword(int sockfd, keyword_t *keyword);
// void send_with_length(int sockfd, char *buf);
// void recv_with_length(int sockfd, char **buf);

// abort the program if we fail a send or received?
void send_message(int sockfd, keyword_t keyword, char *name, char *message);
void recv_message(int sockfd, keyword_t *keyword, char **name, char **message);

#endif