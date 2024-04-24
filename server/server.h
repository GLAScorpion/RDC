#ifndef server_h
#define server_h
#include "../utils/httpreader.h"
#include <arpa/inet.h>
struct Server {
  struct HTTPHeaders headers;
  struct HTTPReader reader;
  struct sockaddr_in server;
  struct sockaddr_in remote;
  int server_socket;
  int remote_socket;
  int isParentProc;
};

int SetupServer(struct Server *server, int port, int queue);

void StartServer(struct Server *server);
#endif
