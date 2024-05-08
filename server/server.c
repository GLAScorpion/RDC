#include "server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int SetupServer(struct Server *server, int port, int queue) {
  struct sigaction sig;
  sig.sa_flags = SA_NOCLDWAIT;
  sigaction(SIGCHLD, &sig, NULL);
  server->isParentProc = -1;
  /*
   * Creo un socket per permettere al programma di effettuare
   * operazioni di rete e connetersi con altri devices
   *
   * */
  server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server->server_socket == -1) {
    return -1;
  }
  int True = 1;
  /*
   * Setto SO_REUSEADDR a 1 (true) per permettere al socket
   * di riutilizzare degli indirizzi già in uso da altri socket.
   * Gli altri socket devono essere però in una fase di wait
   * ossia di inattività
   *
   * */
  int status = setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR,
                          &True, sizeof(int));
  if (status == -1) {
    return -1;
  }
  /*
   * Configuro la struct di rete per il server cosicchè
   * essa utilizzi come ip quello della macchina, ossia lo 0
   * e utilizzi come porta la porta specificata
   *
   * */
  server->server.sin_family = AF_INET;
  server->server.sin_port = htons(port);
  server->server.sin_addr.s_addr = 0;
  /*
   * Bindo il socket secondo la struct appena configurata
   *SA_NOCLDWAIT
   * */
  status = bind(server->server_socket, (struct sockaddr *)&server->server,
                sizeof(struct sockaddr_in));
  if (status == -1) {
    return -1;
  }
  /*
   * Metto il socket in ascolto (listen) di nuove connessioni
   * provenienti dall'esterno, con una queue che definisce il
   * numero massimo di connessioni che possono rimanere in coda
   *
   * */
  status = listen(server->server_socket, queue);
  return 0;
}

int AcceptClient(struct Server *server) {
  socklen_t sck_size = sizeof(struct sockaddr_in);
  /*
   * Accetto le connessioni in entrata in un socket dedicato
   * e carico le informazioni relative al client in una struct
   * sockaddr_in da cui posso eventualmente ricavare l'ip
   *
   * */
  server->remote_socket = accept(server->server_socket,
                                 (struct sockaddr *)&server->remote, &sck_size);
  /*
   * Effettuo una fork del processo per permettere al child di
   * gestire la connessione e al parent di continuare ad
   * accettare connessioni e salvo lo stato di parentela
   * per riconoscere durante l'esecuzione se effettuare certe
   * operazioni di liberazione/rilascio delle risorse
   *
   * */
  server->isParentProc = fork();
  return server->remote_socket;
}

void HandleGetRequest(int socket, struct HTTPReader *reader,
                      struct HTTPHeaders *headers) {
  char default_index[] = "index.html";
  char *filename = default_index;
  if (reader->data.string[reader->second_section + 1] != 0) {
    filename = reader->data.string + reader->second_section + 1;
  }
  FILE *file = fopen(filename, "rb");
  struct Dstring response;
  CreateStr(&response, "HTTP/1.1 ");
  if (file == NULL) {
    ConcatStr(&response, "404 NOT FOUND");
  } else {
    ConcatStr(&response, "200 OK");
  }
  ConcatStr(&response, "\r\n");
  MakeClientRequestHeaders(headers, &response);
  ConcatStr(&response, "\r\n");
  if (file == NULL) {
    ConcatStr(&response, "<html><h1>File ");
    ConcatStr(&response, filename);
    ConcatStr(&response, " non trovato</h1></html>\r\n");
  }
  write(socket, response.string, response.size);
  DestroyStr(&response);
  const int buf_size = 1024;
  char buffer[buf_size];
  size_t actual_read;
  while (!feof(file)) {
    actual_read = fread(buffer, 1, buf_size, file);
    write(socket, buffer, actual_read);
  }
  fclose(file);
}

void HandleHeadRequest(int socket, struct HTTPReader *reader,
                       struct HTTPHeaders *headers) {
  ;
}

void HandlePostRequest(int socket, struct HTTPReader *reader,
                       struct HTTPHeaders *headers) {
  ;
}

void StartServer(struct Server *server) {
  int child_status;
  size_t bytes_read = 0;
  while (1) {
    if (AcceptClient(server) == -1 && !server->isParentProc) {
      exit(-1);
    }
    if (!server->isParentProc) {
      struct HTTPReader reader;
      struct HTTPHeaders headers;
      close(server->server_socket);
      CreateHTTPHeaders(&headers);
      CopyStr(&headers.general.Connection, "close");
      CreateHTTPReader(&reader);
      ReadHeaders(&reader, server->remote_socket);
      // bytes_read = ReadContent(&server->reader, server->remote_socket);
      const char *method = reader.data.string;
      // char log_ip[INET_ADDRSTRLEN];
      //  inet_ntop(AF_INET, &server->remote, log_ip, INET_ADDRSTRLEN);
      char *log_ip = inet_ntoa(server->remote.sin_addr);
      printf("\"%s %s %s\" from address \"%s\"\n", method,
             method + reader.second_section, method + reader.third_section,
             log_ip);
      if (!strcmp(method, Methods[GET])) {
        HandleGetRequest(server->remote_socket, &reader, &headers);
      } else if (!strcmp(method, Methods[HEAD])) {
        HandleHeadRequest(server->remote_socket, &reader, &headers);
      } else if (!strcmp(method, Methods[POST])) {
        HandlePostRequest(server->remote_socket, &reader, &headers);
      }
      close(server->remote_socket);
      DestroyHTTPHeaders(&headers);
      DestroyHTTPReader(&reader);
      exit(-1);
    } else if (server->isParentProc > 0) {
      close(server->remote_socket);
    }
  }
}
