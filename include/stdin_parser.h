#ifndef STDIN_PARSER_H
#define STDIN_PARSER_H

#define HELP_COMMAND "/help"
#define LOGOUT_COMMAND "/logout"
#define LIST_COMMAND "/listu"
#define CHAT_COMMAND "/chat"

#define INVAL_STRING "No such command. Type " HELP_COMMAND \
	" for a list of valid commands.\n"

#define HELP_STRING "\
This is the ME2U chat program. Valid commands are:                          \n\
/help               Displays this help menu.                                \n\
/logout             Logs out from the server and closes all chat windows.   \n\
/listu              Prints a list of users logged in to the server.         \n\
/chat <to> <msg>    Send <msg> to user <to>.                                \n"


void parse_stdin();

#endif  /* STDIN_PARSER_H */
