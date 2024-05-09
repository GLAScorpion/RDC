#include "server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
int SetupServer(struct Server *server, int port, int queue) {
  /*
   *
   * Un child che ha fatto exit() ma non viene waitato diventa
   * uno zombie e il kernel mantiene alcune delle sue relative
   * informazioni.
   * Per evitare ciò e di riempire la ram di informazioni
   * inutili imposto dei parametri sui segnali di terminazione.
   * In particolare setto la flag SA_NOCLDWAIT per il segnale
   * del child in modo tale che dopo la exit esso non diventi
   * uno zombie, in quanto non aspetta una chiamata wait().
   *
   * https://linux.die.net/man/2/waitpid
   * */
  struct sigaction sig;
  sig.sa_flags = SA_NOCLDWAIT;
  /*
   * Setto con sigaction() la flag su SIGCHLD che è il segnale
   * relativo al child.
   *
   * https://linux.die.net/man/2/sigaction
   * */
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
   * operazioni di liberazione/rilascio delle risorse.
   * fork() ritorna 0 al child e il PID del child al parent.
   *
   * */
  server->isParentProc = fork();
  return server->remote_socket;
}

FILE *HandleHeadRequest(int socket, struct HTTPReader *reader,
                        struct HTTPHeaders *headers) {
  /*
   * default_index contiene il nome della risorsa che si
   * accede di default, ossia la locazione "/"
   *
   * */
  char default_index[] = "index.html";
  /*
   * filename contiene il nome effettivo della risorsa
   * che verrà letta
   *
   * */
  char *filename = default_index;
  /*
   * Una corretta request line alla locazione root (/)
   * è contenuta nella data.string del reader alla posizione
   * second_section e il char direttamente successivo è il
   * valore ascii di controllo "\0", ossia l'int 0.
   * Perciò è sufficiente verificare il valore di quella
   * posizione in memoria e se diverso da 0 è possibile
   * sostituire il file di default con il nome della locazione
   * indicata dalla request line, puntando il char subito dopo
   * il char "/" della request
   *
   * */
  if (reader->data.string[reader->second_section + 1] != 0) {
    filename = reader->data.string + reader->second_section + 1;
  }
  /*
   * Apro la risorsa rappresentata da filename in modalità
   * binary così da poter leggere qualsiasi tipo di risorsa
   *
   * */
  FILE *file = fopen(filename, "rb");
  /*
   * response contiene la risposta alla request:
   * Response = Status-Line
   *            *(( general-header
   *             | response-header
   *             | entity-header ) CRLF)
   *             CRLF
   *             [ message-body ]
   *
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   *
   *               HTTP-Version = versione di http che la risposta
   *                              segue
   *               Status-Code = è un codice che indica diverse info
   *                             riguardanti la risposta ed è sempre
   *                             lungo 3 cifre. Ci sono diversi
   *                             significati definiti nell'RFC, ma
   *                             le categorie principali sono:
   *                             1XX: Informational
   *                             2XX: Success
   *                             3XX: Redirection
   *                             4XX: Client Error
   *                             5XX: Server Error
   *               Reason-Phrase = Breve spiegazione del Status-Code
   *
   * */
  struct Dstring response;
  CreateStr(&response, "HTTP/1.1 ");
  /*
   * Se dopo aver tentato di aprire il file ottengo un
   * puntatore NULL significa che la risorsa non esiste o
   * non è accessibile quindi si risponde con un codice
   * di errore 404, ossia risorsa non trovata.
   * In caso contrario si risponde con 200.
   *
   * */
  if (file == NULL) {
    ConcatStr(&response, "404 NOT FOUND");
  } else {
    ConcatStr(&response, "200 OK");
  }
  ConcatStr(&response, "\r\n"); // Status-Line completa
  /*
   * Predispongo il salvataggio della dimensione del body
   * per poterlo scrivere nel Content-Length header
   *
   * */
  size_t body_size = 0;
  struct Dstring response_body;
  CreateStr(&response_body, "\r\n"); // CRLF che precede il body
  /*
   * Se il file non esiste stampo una pagina di errore
   * che comunica al client che la risorsa non è stata
   * trovata. Questo avviene solo se il method della
   * request non è HEAD.
   * In caso contrario trovo la dimensione del file e
   * la salvo da scrivere nel Content-Header
   * */
  if (file == NULL) {
    ConcatStr(&response_body, "<html><h1>File ");
    ConcatStr(&response_body, filename);
    ConcatStr(&response_body, " non trovato</h1></html>\r\n");
    body_size = response_body.size;
  } else {
    struct stat file_info;
    stat(filename, &file_info);
    body_size = file_info.st_size;
  }
  /*
   * Scrivo la dimensione del body nell'header
   * Content-Length, poi creo gli headers e li
   * appendo alla response
   *
   * */
  CreateStrFromInt(&headers->entity.Content_Length, body_size);
  MakeClientRequestHeaders(headers, &response);
  // entro solo se non è HEAD
  if (strcmp(reader->data.string, Methods[HEAD])) {
    ConcatStr(&response, response_body.string);
  }
  /*
   * Scrivo la response e libero le risorse allocate
   * dinamicamente
   *
   * */
  write(socket, response.string, response.size);
  DestroyStr(&response_body);
  DestroyStr(&response);
  /*
   * Restituisco il puntatore al file per permetterne
   * la chiusura o il riutilizzo da parte della
   * HandleGetRequest
   *
   * */
  return file;
}

void HandleGetRequest(int socket, struct HTTPReader *reader,
                      struct HTTPHeaders *headers) {
  /*
   * riutilizzo il format della HEAD request e salvo
   * il puntatore al file. Se il file non esiste
   * salto la fase di scrittura
   * */
  FILE *file = HandleHeadRequest(socket, reader, headers);
  if (file == NULL) {
    return;
  }
  /*
   * Uso un buffer di un KB per leggere il file e
   * scriverlo al client.
   * actual_read contiene i byte letti effettivamente
   * dalla read del file.
   * fread(buffer, dimensione in byte degli oggetti,
   *       numero degli oggetti, file )
   *
   * */
  const int buf_size = 1024;
  char buffer[buf_size];
  size_t actual_read;
  while (!feof(file)) {
    actual_read = fread(buffer, 1, buf_size, file);
    write(socket, buffer, actual_read);
  }
  fclose(file);
}

void HandlePostRequest(int socket, struct HTTPReader *reader,
                       struct HTTPHeaders *headers) {
  /*
   * La POST non è ancora stata implementata/non ha
   * ancora una funzione perciò rispondo al client
   * con l'errore del server 501 che indica proprio
   * Not Implemented
   *
   * */
  char response[] = "HTTP/1.1 501 NOT IMPLEMENTED\r\n\r\n<html><h1>OPERATION "
                    "NOT ALLOWED</h1></html>\r\n";
  write(socket, response, sizeof(response));
}

void HandleBadRequest(int socket) {
  /*
   * Gestisco la Bad Request con il codice di errore
   * del client (400).
   *
   * */
  char response[] = "HTTP/1.1 400 BAD REQUEST\r\n\r\n<html><h1>400 BAD "
                    "REQUEST</h1></html>\r\n";
  write(socket, response, sizeof(response));
}

void StartServer(struct Server *server) {
  /*
   * Il server funziona in loop perenne e accetta sempre
   * nuove connessioni e effettua fork per gestirle.
   * I child si occuperanno di gestire le connessioni
   * mentre il parent rimane in ascolto.
   *
   * */
  while (1) {
    // se è avvenuta una fork ma il socket remoto è
    // problematico e mi trovo nel child esso viene
    // terminato immediatamente in errore
    if (AcceptClient(server) == -1 && !server->isParentProc) {
      exit(-1);
    }
    if (!server->isParentProc) {
      // ZONA DEL CHILD
      /*
       * Alloco HTTPReader e HTTPHeaders nel child così
       * da non avere problemi tra memoria allocata
       * dinamicamente e CoW (Copy-On-Write)
       *
       * */
      struct HTTPReader reader;
      CreateHTTPReader(&reader);
      struct HTTPHeaders headers;
      CreateHTTPHeaders(&headers);
      // Chiudo il socket del server per il child
      // perchè non è più utile
      close(server->server_socket);
      CopyStr(&headers.general.Connection, "close");
      // Comincio la lettura della request del client
      ReadHeaders(&reader, server->remote_socket);
      // Salvo il puntatore al metodo
      const char *method = reader.data.string;
      // Loggo l'ip del client e lo stampo insieme
      // alla request line
      char *log_ip = inet_ntoa(server->remote.sin_addr);
      printf("\"%s %s %s\" from address \"%s\"\n", method,
             method + reader.second_section, method + reader.third_section,
             log_ip);
      /*
       * Comparo le stringhe del metodo e gestisco la
       * richiesta coerentemente.
       * A compare fallita considero la request come
       * incorretta e la gestisco con HandleBadRequest
       *
       * */
      if (!strcmp(method, Methods[GET])) {
        HandleGetRequest(server->remote_socket, &reader, &headers);
      } else if (!strcmp(method, Methods[HEAD])) {
        fclose(HandleHeadRequest(server->remote_socket, &reader, &headers));
      } else if (!strcmp(method, Methods[POST])) {
        HandlePostRequest(server->remote_socket, &reader, &headers);
      } else {
        HandleBadRequest(server->remote_socket);
      }
      /*
       * Terminate le operazioni di risposta chiudo il
       * socket, libero le risorse allocate dinamicamente
       * ed esco con exit(1)
       *
       * */
      close(server->remote_socket);
      DestroyHTTPHeaders(&headers);
      DestroyHTTPReader(&reader);
      exit(1);
    } else if (server->isParentProc > 0) {
      // ZONA DEL PARENT
      /*
       * chiudo il socket remoto perchè al parent
       * non serve più e verrà gestito dal child
       *
       * */
      close(server->remote_socket);
    }
  }
}
