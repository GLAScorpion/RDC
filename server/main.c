#include "server.h"
#include <stdio.h>
int main() {

  struct Server server;
  if (SetupServer(&server, 7070, 5)) {
    perror("Error: ");
  }
  printf("Connection established on fd: %d\n", server.server_socket);
  StartServer(&server);
}
