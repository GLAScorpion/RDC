#include "client.h"
#include <string.h>
int connect_client(struct Client *client, const char *ip, int port) {

  client->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client->socket == -1) {
    return -1;
  }

  client->server.sin_family = AF_INET;
  client->server.sin_port = htons(port);
  client->server.sin_addr.s_addr = inet_addr(ip);
  return connect(client->socket, (struct sockaddr *)&client->server,
                 sizeof(struct sockaddr_in));
}

int make_request(struct Client *client, const char *request) {
  char msg[1000] = "GET ";
  const char tail[] = " HTTP/1.0\r\n\r\n";
  strcat(msg, request);
  strcat(msg, tail);
  return write(client->socket, msg, strlen(msg));
}

int read_all(struct Client *client) {
  int total = 0;
  char buf[128] = {0};
  while (1) {
    int ret = read(client->socket, buf, sizeof(buf) - 1);
    if (ret > 0) {
      strcat(client->response, buf);
      total += ret;
      memset(buf, 0, sizeof(buf));
    } else {
      return total;
    }
  }
  return total;
}
