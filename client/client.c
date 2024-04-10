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
