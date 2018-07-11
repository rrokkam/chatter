#ifndef NETWORK_PARSER_H
#define NETWORK_PARSER_H

#include <stdbool.h>

#define MAX_MSG_LEN 3000
#define BUFF_FACTOR 16
#define ME2U_TERM "\n"
#define TERM_LEN 4
#define NUM_KEYS 16

#define FOREACH_KEYWORD(WORD) \
    WORD(ME2U)      \
    WORD(U2EM)      \
    WORD(IAM)       \
    WORD(ETAKEN)    \
    WORD(MAI)       \
    WORD(MOTD)      \
    WORD(LISTU)     \
    WORD(UTSIL)     \
    WORD(TO)        \
    WORD(OT)        \
    WORD(FROM)      \
    WORD(MORF)      \
    WORD(EDNE)      \
    WORD(UOFF)      \
    WORD(BYE)       \
    WORD(EYB)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum keyword_enum {
    FOREACH_KEYWORD(GENERATE_ENUM)
} keyword_t;

static const char *keyword_string[] = {
    FOREACH_KEYWORD(GENERATE_STRING)
};

typedef struct msg {
    keyword_t keyword;
    char *name;
    char *payload;
    char *bufptr;
} msg_t;

char *keyword_to_string(keyword_t keyword);
int string_to_keyword(char *string);

msg_t *set_message(msg_t *msg, keyword_t keyword, char *name, char *payload);
void free_message(msg_t *ptr);

msg_t *me2u_recv(int sockfd, char *term);

#endif