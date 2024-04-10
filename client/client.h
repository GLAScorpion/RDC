#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

enum Method { GET = 0, POST, HEAD };
static const char *const Methods[] = {"GET ", "POST ", "HEAD "};

struct Client {
  struct sockaddr_in server;
  char response[2000000];
  int socket;
};

int connect_client(struct Client *client, const char *ip, int port);

int make_request(struct Client *client, enum Method method, const char *request,
                 const char *body);

int read_all(struct Client *client);
