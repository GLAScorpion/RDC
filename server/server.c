#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int SetupServer(struct Server *server, int port, int queue) {
  server->isParentProc = -1;
  server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server->server_socket == -1) {
    return -1;
  }
  int True = 1;
  int status = setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR,
                          &True, sizeof(int));
  if (status == -1) {
    return -1;
  }
  server->server.sin_family = AF_INET;
  server->server.sin_port = htons(port);
  server->server.sin_addr.s_addr = 0;
  status = bind(server->server_socket, (struct sockaddr *)&server->server,
                sizeof(struct sockaddr_in));
  if (status == -1) {
    return -1;
  }
  status = listen(server->server_socket, queue);
  return 0;
}

int AcceptClient(struct Server *server) {
  socklen_t sck_size = sizeof(struct sockaddr_in);
  server->remote_socket = accept(server->server_socket,
                                 (struct sockaddr *)&server->remote, &sck_size);
  server->isParentProc = fork();
  return server->remote_socket;
}

void HandleGetRequest(struct Server *server) {
  char default_index[] = "index.html";
  char *filename = default_index;
  if (server->reader.data.string[server->reader.second_section + 1] != 0) {
    filename = server->reader.data.string + server->reader.second_section + 1;
  }
  FILE *file = fopen(filename, "rt");
  struct Dstring response;
  CreateStr(&response, "HTTP/1.1 ");
  if (file == NULL) {
    ConcatStr(&response, "404 NOT FOUND");
  } else {
    ConcatStr(&response, "200 OK");
  }
  ConcatStr(&response, "\r\n");
  MakeClientRequestHeaders(&server->headers, &response);
  ConcatStr(&response, "\r\n");
  if (file == NULL) {
    ConcatStr(&response, "<html><h1>File ");
    ConcatStr(&response, filename);
    ConcatStr(&response, " non trovato</h1></html>\r\n");
  }
  write(server->remote_socket, response.string, response.size);
  DestroyStr(&response);
  const int buf_size = 1024;
  char buffer[buf_size];
  int actual_read;
  while (!feof(file)) {
    actual_read = fread(buffer, 1, buf_size, file);
    write(server->remote_socket, buffer, actual_read);
  }
  fclose(file);
}

void HandleHeadRequest(struct Server *server) { ; }

void HandlePostRequest(struct Server *server) { ; }

void StartServer(struct Server *server) {
  CreateHTTPHeaders(&server->headers);
  CopyStr(&server->headers.general.Connection, "close");
  size_t bytes_read = 0;
  while (1) {
    if (AcceptClient(server) == -1 && !server->isParentProc) {
      exit(-1);
    }
    if (!server->isParentProc) {
      close(server->server_socket);
      CreateHTTPReader(&server->reader);
      ReadHeaders(&server->reader, server->remote_socket);
      // bytes_read = ReadContent(&server->reader, server->remote_socket);
      const char *method = server->reader.data.string;
      // char log_ip[INET_ADDRSTRLEN];
      //  inet_ntop(AF_INET, &server->remote, log_ip, INET_ADDRSTRLEN);
      char *log_ip = inet_ntoa(server->remote.sin_addr);
      printf("%s %s %s from address %s\n", method,
             method + server->reader.second_section,
             method + server->reader.third_section, log_ip);
      if (!strcmp(method, Methods[GET])) {
        HandleGetRequest(server);
      } else if (!strcmp(method, Methods[HEAD])) {
        HandleHeadRequest(server);
      } else if (!strcmp(method, Methods[POST])) {
        HandlePostRequest(server);
      }
      close(server->remote_socket);
      DestroyHTTPHeaders(&server->headers);
      DestroyHTTPReader(&server->reader);
      exit(-1);
    } else if (server->isParentProc > 0) {
      close(server->remote_socket);
    }
  }
}
