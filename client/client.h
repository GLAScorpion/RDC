#ifndef client_h
#define client_h
#include "../utils/dstring.h"
#include "../utils/httpheaders.h"
#include "../utils/httpreader.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

struct Client {
  struct HTTPReader reader;
  struct sockaddr_in server;
  struct Dstring response;
  int socket;
  int last_method;
};

void create_client(struct Client *client);

int connect_client(struct Client *client, const char *ip, int port);

int make_request(struct Client *client, enum Method method, const char *request,
                 struct HTTPHeaders *headers, const char *body);

int read_all(struct Client *client);

void close_client(struct Client *client);
#endif
