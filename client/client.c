#include "client.h"
#include <string.h>
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
                 const char *body) {
<<<<<<< HEAD
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
   *    GET: Serve per richiedere delle risorse, identificate dalla Request-URI.
   *         Il server deve restituire una risposta con un body.
   *    HEAD: Come GET ma il server non deve restituire un Entity-Body.
   *    POST: Segnala al server che deve accettare il contenuto dell'Entity-Body
   *          nella request. E' necessario includere nella request l'header
   *          Content-Length che indica la lunghezza in bytes dell'Entity-Body.
   *
   * General-Header:
   *    Date = "Date:" SP HTTP-Date
   *           HTTP-Date = wkday "," SP day SP month SP year SP time SP "GMT"
   *    Pragma = "Pragma:" SP pragma-directive
   *             direttive specifiche dell'implementazione
   *
   * Request-Header:
   *    Authorization = "Authorization:" SP credentials
   *    From = "From:" SP e-mail
   *    If-Modified-Since = "If-Modified-Since:" SP HTTP-Date
   *                        Se il contenuto specificato dalla GET non è stato
   *                        modificato a partire dalla HTTP-Date fino ad adesso
   *                        il server non deve restituire risorse, ma piuttosto
   *                        il codice di stato 304 nella risposta
   *    Referer = "Referer:" SP URI
   *              Indica la risorsa da cui il client ha ottenuto la nuova
   * Request-URI Per esempio il Referer è la pagina da cui poi clicchi un
   *              collegamento per raggiungere una nuova pagina
   *    User-Agent = "User-Agent:" SP User-Agent-String
   *                 Stringa che può interessare al server e descrive il nome
   *                 del programma che il client usa per la connessione
   *
   * Entity-Header:
   *    Allow: "Allow:" SP method
   *           Indica i methodi possibili per una risorsa
   *    Content-Encoding: "Content-Encoding:" SP encoding
   *                      Indica eventuale encoding aggiuntivo, tipo
   * compressione Content-Length: "Content-Length:" SP length La lunghezza in
   * bytes del Entity-Body Content-Type: "Content-Type:" SP MIME-type Indica il
   * MIME type dei dati trasmessi Expires: "Expires:" SP HTTP-Date Indica la
   * data dopo la quale la risorsa è da considerare vecchia e da riscaricare
   *    Last-Modified: "Last-Modified:" SP HTTP-Date
   *                   Indica una data che il client ritiene essere di ultima
   *                   modifica della risorsa Entity-Body. In base poi
   *                   all'implementazione il server può decidere di aggiornare
   *                   i propri dati se più vecchi di quelli forniti dal client
   * */

  char Request[1000];
  strcpy(Request, Methods[method]); // Method
  strcat(Request, request);         // Request-URI
  strcat(Request, " HTTP/1.0\r\n"); // HTTP-Version
  int body_len = strlen(body);
  if (body_len > 0) { // Entity-Header
                      // Content-Type
    const char content_type[] = "Content-Type: text/plain; charset=utf8\r\n";
    strcat(Request, content_type);
    // Content-Length
    const char content_len[] = "Content-Length: ";
    strcat(Request, content_len);
    sprintf(Request + strlen(Request), "%d\r\n\r\n", body_len); // CRLF
                                                                // Entity-Body
    strcat(Request, body);
    strcat(Request, "\r\n"); // CRLF
  }
  strcat(Request, "\r\n"); // CRLF
  // printf("%s", Request);
  return write(client->socket, Request, strlen(Request));
=======
  char request_line[1000];
  strcpy(request_line, Methods[method]);
  strcat(request_line, request);
  const char entity_header[] = " HTTP/1.1";
  strcat(request_line, entity_header);
  int body_len = strlen(body);
  if (body_len > 0) {
    const char content_type[] = "\r\nContent-Type: text/plain; charset=utf8";
    strcat(request_line, content_type);
    const char content_len[] = "\r\nContent-Length: ";
    strcat(request_line, content_len);
    sprintf(request_line + strlen(request_line), "%d\r\n\r\n", body_len);
    strcat(request_line, body);
  }
  strcat(request_line, "\r\n\r\n");
  // printf("%s", request_line);
  return write(client->socket, request_line, strlen(request_line));
>>>>>>> refs/remotes/origin/main
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
