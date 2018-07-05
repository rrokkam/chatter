#include <sys/socket.h>
#include "protocol_new.h"

keyword_t keyword_buf;
char *name_buf;
char *message_buf;

void init() {
	keyword_buf = malloc(sizeof keyword_t);
	name_buf = malloc(MAX_NAME_LEN);
	message_buf = malloc(MAX_MESSAGE_LEN);
}

void fini() {
	free(keyword_buf);
	free(name_buf);
	free(message_buf);
}

bool expect_name(keyword_t keyword) {
	switch (keyword) {
		case LGIN: case EUSR: case SEND: case RECV: case LGOT: return true;
		case MOTD: case LIST: return false;
	}
}

bool expect_message(keyword_t keyword) {
	switch (keyword) {
		case MOTD: case LIST: case SEND: return true;
		case LGIN: case EUSR: case RECV: case LGOT: return false;
	}
}

void send_keyword(int sockfd, keyword_t keyword) {
//	char *keyword_string = NULL;  // give error on invalid keyword
	// switch (keyword) {
	// 	case LGIN: keyword_string = "LGIN"; break;
	// 	case MOTD: keyword_string = "MOTD"; break;
	// 	case EUSR: keyword_string = "EUSR"; break;
	// 	case LIST: keyword_string = "LIST"; break;
	// 	case SEND: keyword_string = "SEND"; break;
	// 	case RECV: keyword_string = "RECV"; break;
	// 	case LGOT: keyword_string = "LGOT"; break;
	// }	
	send(sockfd, keyword, sizeof keyword_t, 0);
}

void recv_keyword(int sockfd, keyword_t *keyword) {
	keyword = &keyword_buf;
//	char *keyword_string = NULL;
	recv(sockfd, keyword, sizeof keyword_t, 0);
	// if (!strncmp(keyword_string, "LGIN", KEYWORD_STRING_LEN)) {
	// 	keyword = LGIN;
	// } else if (!strncmp(keyword_string, "MOTD", KEYWORD_STRING_LEN)) {
	// } else if (!strncmp(keyword_string, "EUSR", KEYWORD_STRING_LEN)) {
	// } else if (!strncmp(keyword_string, "LIST", KEYWORD_STRING_LEN)) {
	// } else if (!strncmp(keyword_string, "SEND", KEYWORD_STRING_LEN)) {
	// } else if (!strncmp(keyword_string, "RECV", KEYWORD_STRING_LEN)) {
	// } else if (!strncmp(keyword_string, "LGOT", KEYWORD_STRING_LEN)) {
	// }
}

void send_with_length(int sockfd, char *buf) {

}

void recv_with_length(int sockfd, char **buf) {

}

// give keyword, name, message arguments or just use them via the globals
// need to add handling for when send/recv return an error
// where to do the length checking for arguments?
void send_message(int sockfd, keyword_t keyword, char *name, char *message) {
	send_keyword(sockfd, keyword);
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
	recv_keyword(sockfd, keyword_buf);
	if (expect_name(keyword_buf)) {
		recv_with_length(sockfd, name_buf);
	}
	if (expect_message(keyword_buf)) {
		recv_with_length(sockfd, message_buf);
	}
}
