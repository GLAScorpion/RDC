#ifndef httpheaders_h
#define httpheaders_h
#include "dstring.h"
struct HTTPHeaders {
  struct HGeneral {
    /*
     * General-Header:
     *   Connection: "Connection:" SP "close", "keepalive"
     *       Serve per mantenere attiva la connesione server
     *       client e per permettere a quest'ultimo di fare
     *       ulteriori request
     *
     * */
    struct Dstring Connection;
    /*
     *    Date = "Date:" SP HTTP-Date
     *        HTTP-Date = wkday "," SP day SP month SP year SP time SP "GMT"
     * */
    struct Dstring Date;
    /*
     *    Pragma = "Pragma:" SP pragma-directive
     *             direttive specifiche dell'implementazione
     * */
    struct Dstring Pragma;
    /*
     *    Transfer-Encoding: "Transfer-Encoding:" SP encoding
     *       Serve per decidere come vengono trasferiti i dati.
     *       Una dei metodi è quello chunked
     *
     * */
    struct Dstring Transfer_Encoding;
  } general;

  struct HRequest {
    /*
     * Request-Header:
     *    Accept-Encoding: gzip | compress | deflate | br| identity| *
     * */
    struct Dstring Accept_Encoding;
    /*
     *    Authorization = "Authorization:" SP credentials
     * */
    struct Dstring Authorization;
    //    From = "From:" SP e-mail
    struct Dstring From;
    /*
     *    If-Modified-Since = "If-Modified-Since:" SP HTTP-Date
     *        Se il contenuto specificato dalla GET non è stato
     *        modificato a partire dalla HTTP-Date fino ad adesso
     *        il server non deve restituire risorse, ma piuttosto
     *        il codice di stato 304 nella risposta.
     * */
    struct Dstring If_Modified_Since;
    /*
     *    Referer = "Referer:" SP URI
     *        Indica la risorsa da cui il client ha ottenuto la nuova
     * Request-URI Per esempio il Referer è la pagina da cui poi clicchi
     * un collegamento per raggiungere una nuova pagina.
     * */
    struct Dstring Referer;
    /*
     *   User-Agent = "User-Agent:" SP User-Agent-String
     *       Stringa che può interessare al server e
     *       descrive il nome del programma che il client
     *       usa per la connessione.
     * */
    struct Dstring User_Agent;
  } request;

  struct HEntity {
    /*
     * Entity-Header:
     *   Allow: "Allow:" SP method
     *       Indica i methodi possibili per una risorsa.
     * */
    struct Dstring Allow;
    /*
     *   Content-Encoding: "Content-Encoding:" SP encoding
     *       Indica eventuale encoding aggiuntivo, tipo compressione.
     * */
    struct Dstring Content_Encoding;
    /*
     *   Content-Length: "Content-Length:" SP Length
     *       Indica la lunghezza del body in byte
     * */
    struct Dstring Content_Length;
    /*
     *   Content-Type: "Content-Type:" SP MIME-type
     *       Indica il MIME type dei dati trasmessi.
     * */
    struct Dstring Content_Type;
    /*
     *   Expires: "Expires:" SP HTTP-Date
     *       Indica la data dopo la quale la risorsa è da considerare vecchia
     *       e da riscaricare.
     * */
    struct Dstring Expires;
    /*
     *   Last-Modified: "Last-Modified:" SP HTTP-Date
     *       Indica una data che il client ritiene essere di ultima
     *       modifica della risorsa Entity-Body. In base poi
     *       all'implementazione il server può decidere di aggiornare
     *       i propri dati se più vecchi di quelli forniti dal client.
     * */
    struct Dstring Last_Modified;
  } entity;

  struct HExtra {
    /*
     * Extra:
     *   Extra header che non finiscono in una categoria
     *   oppure non ho trovato documentazione a riguardo
     *
     * */
  } extra;
};

struct header {
  long long int name;
  long long int value;
};

void CreateHTTPHeaders(struct HTTPHeaders *headers);

void MakeClientRequestHeaders(struct HTTPHeaders *headers,
                              struct Dstring *request);

void PopulateHTTPHeaders(struct HTTPHeaders *headers, struct header *header_pos,
                         const char *header_data, size_t size);
#endif
