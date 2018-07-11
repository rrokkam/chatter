#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "protocol.h"
#include "network_parser.h"
#include "stdin_parser.h"
#include "wrappers.h"
#include "debug.h"

// todo: server terminates, we need to handle sigpipe on read?

#define ME2U_SPECIAL_CHARS " \n"
#define MAX_EPOLL_EVENTS 10
#define MAX_USERNAME_LENGTH 10
#define MIN_PORT 1025

void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1) {
        // when reading, the client will hit a read of 
        // length 0, which is an EOF, and close the FD. 
        // If the handler interrupts read(), then we 
        // will try again, and get EBADF since the FD is 
        // closed. (race condition on who closes first!!)
    }
}

void run(char *name, char *server_ip, int server_port) {
    int sockfd, epfd;
    sockfd = client_connect(server_ip, server_port);

    login(sockfd, name);

    // stack is OK, since epoll never leaves this function.
    struct epoll_event ev, events[MAX_EPOLL_EVENTS];
    ev.events = EPOLLIN;
    epfd = Epoll_create(EPOLL_CLOEXEC);

    ev.data.fd = STDIN_FILENO;
    Epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ev.data.fd = sockfd;
    Epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    int num_events, i, infd;
    msg_t *server_msg;
    for ( ; ; ) {
        num_events = Epoll_wait(epfd, events, MAX_EPOLL_EVENTS, -1);
        for (i = 0; i < num_events; i++) {
            infd = events[i].data.fd;
            if (infd == STDIN_FILENO) {
                parse_stdin(sockfd);
            } else if (infd == sockfd) {
                msg_t *server_msg = me2u_recv(sockfd, ME2U_TERM);
                switch (server_msg->keyword) {
                    case FROM:
                        from_handler(sockfd, server_msg);
                        break;
                    case UOFF:
                        uoff_handler(server_msg);
                        break;
                    default:
                        // got some garbage? should probably exit here..
                        debug("Got an unexpected message keyword.")
                        exit(EXIT_FAILURE);
                        break;
                }
            }
        }
    }
}

/**
 * Start the me2u client program.
 */
int main(int argc, char *argv[]) {
    int arg;
    while ((arg = getopt(argc, argv, "h")) != -1) {
        switch(arg) {
            case 'h':
                printf(USAGE_STRING);
                exit(EXIT_SUCCESS);
            default:  // getopt will print the invalid argument out.
                printf(USAGE_STRING);
                exit(EXIT_FAILURE);
        }
    }

    // arguments must be: programname username ip port
    if (argc != 4) {
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    }

    char *name = argv[optind];
    char *server_ip = argv[optind + 1];
    int server_port = atoi(argv[optind + 2]);

    // len should be less than or equal to 10 chars, no space or rnrn
    int namelen = strlen(name);
    if (namelen > MAX_USERNAME_LENGTH || strcspn(name, ME2U_SPECIAL_CHARS) != namelen) {
        debug("Invalid username.");
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    } 

    if (server_port < MIN_PORT) { // atoi gives 0 in error
        debug("Invalid port.");
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    }

    struct sigaction action, oldaction;
    action.sa_handler = sigchld_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGCHLD, &action, &oldaction);

    run(name, server_ip, server_port);

    sigaction(SIGCHLD, &oldaction, NULL);
    exit(EXIT_SUCCESS);
}
