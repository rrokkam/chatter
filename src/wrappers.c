#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>

#include "debug.h"
#include "wrappers.h"

void *Malloc(size_t size) {
    void *ret;
    if ((ret = malloc(size)) == NULL) {
        debug(1, "Failed a malloc of size %d", (int) size);
        exit(EXIT_FAILURE);
    }
    return ret;
}

void *Realloc(void *ptr, size_t size) {
    void *ret;
    if ((ret = realloc(ptr, size)) == NULL) {
        debug(1, "Failed a realloc of size %d on %p", (int) size, ptr);
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Epoll_create(int flags) {
    int ret;
    if ((ret = epoll_create1(flags)) < 0) {
        debug(1, "Failed to create an epoll instance.");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event event[]) {
    int ret;
    if ((ret = epoll_ctl(epfd, op, fd, event)) < 0) {
        debug(1, "Illegal epoll modification requested.");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int ret;
    while ((ret = epoll_wait(epfd, events, maxevents, timeout)) < 0) {
        if (errno == EINTR) {
            continue;
        }
        debug(1, "Error occurred while waiting for a socket.");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Send(int sockfd, const void *buf, size_t len, int flags) {
    int ret;
    do {
        ret = send(sockfd, buf, len, flags);
    } while (ret == -1 && errno == EINTR);
    return ret;
}

int Recv(int sockfd, void *buf, size_t len, int flags) {
    int ret;
    do {
        ret = recv(sockfd, buf, len, flags);
    } while (ret == -1 && errno == EINTR);
    return ret;
}
