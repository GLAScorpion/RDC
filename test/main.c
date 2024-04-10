#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // resolve local host
  //
  struct hostent *server;
  server = gethostbyname("142.251.209.36");
  if (server == NULL) {
    perror("gethostbyname");
    return -1;
  }

  printf("gethostbyname success\n");

  // create a socket
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd < 0) {
    perror("socket");
    return -1;
  }

  printf("socket fd = %d\n", fd);
  // connect the socket to localhost

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(&(addr.sin_zero), '\0', 8);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    return -1;
  }

  printf("connect success\n");

  // read html content from localhost

  const char *request =
      "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";

  if (send(fd, request, strlen(request), 0) == -1) {
    perror("send");
    close(fd);
    exit(EXIT_FAILURE);
  }
  printf("send success\n");
  char buf[1024] = {0};
  while (1) {
    int ret = read(fd, buf, sizeof(buf) - 1);
    if (ret > 0) {
      printf("%s", buf);
      memset(buf, 0, sizeof(buf));
    } else {
      break;
    }
  }
  close(fd);

  return 0;
}
