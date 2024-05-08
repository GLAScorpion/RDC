#include "client.h"
#include <string.h>

void create_client(struct Client *client) { CreateHTTPReader(&client->reader); }

int connect_client(struct Client *client, const char *ip, int port) {
  /*
   * AF_INET è la categoria di protocolli Internet per la trasmissione
   * dei dati.
   * SOCK_STREAM e SOCK_DGRAM sono i 2 tipi di socket disponibili.
   * SOCK_STREAM viene usato quando è importante che i dati raggiungano
   * la destinazione completi e in un ordine ben definito, come
   * nel caso di un file. Il protocollo sottostante è il TCP.
   * SOCK_DGRAM viene usato in scenari dove la velocità di trasmissione
   * dei dati ha la priorità sulla completezza, per esempio nello
   * streaming video. Il protocollo infatti non garantisce che i dati
   * arrivino a destinazione completi o che arrivino in generale
   * e non viene tantomeno garantito l'ordine di arrivo. Il protocollo
   * sottostante è spesso l'UDP
   * */
  client->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client->socket == -1) {
    return -1;
  }

  client->server.sin_family = AF_INET;
  /*
   * Una certa porta di rete è un numero di 16 bit dove un server
   * fornisce un certo servizio e certi servizi ampiamente
   * utilizzati associano una porta di "default" o in generale una più
   * convenzionale. Questo non impedisce di fornire un certo servizio
   * su una porta diversa, ma probabilmente il client necessiterà di
   * configurazione aggiuntiva per specificare tali modifiche
   * */
  client->server.sin_port = htons(port);
  /*
   * L'ip è un indirizzo di rete che identifica unicamente ogni
   * macchina collegata
   * */
  client->server.sin_addr.s_addr = inet_addr(ip);
  /*
   * Per instanziare una connessione nel modello client-server
   * è necessario quindi un server che accetti la connessione ad una
   * certa porta e nel client necessitiamo un socket inizializzato.
   * La funzione connect restituisce 0 in caso di connessione con
   * successo e -1 altrimenti
   * */
  return connect(client->socket, (struct sockaddr *)&client->server,
                 sizeof(struct sockaddr_in));
}

int make_request(struct Client *client, enum Method method, const char *request,
                 struct HTTPHeaders *headers, const char *body) {
  /*
   * SP = space
   * CR = \r
   * LF = \n
   *
   * Request-Line
   * *( General-Header      // "*" vuol dire che questo componente può essere
   *  | Request-Header      // presente 0 o + volte
   *  | Entity-Header )
   *  CRLF
   *  [ Entity-Body ]
   *  CRLF
   *
   * Ogni riga, ogni header va a capo con un CRLF, i CRLF specificati nel
   * formato della Request sono aggiuntivi (righe vuote)
   *
   * Request-Line = Method SP Request-URI SP HTTP-Version
   *
   * Methods:
   *    GET: Serve per richiedere delle risorse, identificate dalla
   * Request-URI. Il server deve restituire una risposta con un body.
   *    HEAD: Come GET ma il server non deve restituire un Entity-Body.
   *    POST: Segnala al server che deve accettare il contenuto dell'Entity-Body
   *          nella request. E' necessario includere nella request l'header
   *          Content-Length che indica la lunghezza in bytes dell'Entity-Body.
   * */
  client->last_method = method;
  struct Dstring Request;
  CreateStr(&Request, Methods[method]); // Method
  ConcatStr(&Request, " ");
  ConcatStr(&Request, request);         // Request-URI
  ConcatStr(&Request, " HTTP/1.1\r\n"); // HTTP-Version
  int body_len = strlen(body);
  if (body_len > 0) {
    //"text/plain; charset=utf8";
    CopyIntToStr(&headers->entity.Content_Length, strlen(body));
    MakeClientRequestHeaders(headers, &Request);
    ConcatStr(&Request, "\r\n"); // CRLF
    ConcatStr(&Request, body);   // Entity-Body
  } else {
    MakeClientRequestHeaders(headers, &Request);
    ConcatStr(&Request, "\r\n");
  }
  ConcatStr(&Request, "\r\n"); // CRLF
  printf("%s", Request.string);
  int result = write(client->socket, Request.string, Request.size);
  DestroyStr(&Request);
  return result;
}

void print_headers(struct Client *client) {
  for (int i = 0; i < client->reader.size; i++) {
    printf("%s: %s",
           &client->reader.data.string[client->reader.headers_data[i].name],
           &client->reader.data.string[client->reader.headers_data[i].value]);
  }
}

int read_all(struct Client *client) {
  ReadHeaders(&client->reader, client->socket);
  if (client->last_method == HEAD) {
    return 0;
  }
  return ReadContent(&client->reader, client->socket);
}

void close_client(struct Client *client) {
  DestroyHTTPReader(&client->reader);
  close(client->socket);
}
