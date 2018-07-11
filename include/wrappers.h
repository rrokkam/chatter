#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <sys/socket.h>
#include <sys/epoll.h>

void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);

int Epoll_create(int flags);
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

int Send(int sockfd, const void *buf, size_t len, int flags);
int Recv(int sockfd, void *buf, size_t len, int flags);

int Socket(int domain, int type, int protocol);
int Connect(int sockfd, struct sockaddr *addr, socklen_t addrlen);

#endif /* WRAPPERS_H */
