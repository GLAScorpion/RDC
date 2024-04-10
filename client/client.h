#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

struct client {
  struct sockaddr_in server;
  char response[2000000];
  int socket;
};
