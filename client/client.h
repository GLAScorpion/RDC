#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

struct Client {
  struct sockaddr_in server;
  char response[2000000];
  int socket;
};

int connect_client(struct Client *client, const char *ip, int port);

int make_request(struct Client *client, const char *request);

int read_all(struct Client *client);
