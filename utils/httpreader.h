#ifndef httpreader_h
#define httpreader_h
#include "dstring.h"
#include "httpheaders.h"

struct HTTPReader {
  /*
   * headers_data: Contiene gli offset dei vari headers.
   *               Il campo name contiene l'offset al nome dell'header.
   *               Il campo value l'offset al suo valore.
   *               Gli offset sono da valutare sul campo data
   * */
  struct header *headers_data;
  /*
   * data: Contiene i dati letti semi-raw, ossia le varie
   *       componenti sono separate da zeri, ma senza
   *       ulteriori modifiche. Gestione con Dstring.
   * */
  struct Dstring data;
  /*
   * alloc_size: Dimensione della memoria allocata dinamicamente
   *             relativa a headers_data.
   * size: Dati effettivamente contenuti e validi in headers_data
   * */
  size_t alloc_size;
  size_t size;
  /*
   * parsed_headers: Oggetto di tipo HTTPHeaders che contiene gli
   *                 headers letti e organizzati in maniera più
   *                 comoda ed esplicita per l'utilizzo.
   *                 Eventuali headers non supportati dalla
   *                 libreria HTTPHeaders non sono ovviamente
   *                 reperibili in questo oggetto, ma possono
   *                 essere comunque acceduti manualmente tramite
   *                 headers_data
   *
   * */
  struct HTTPHeaders parsed_headers;
  /*
   * *_section: Corrispondono agli offset delle varie sezioni
   *            che caratterizzano la prima linea di qualsiasi
   *            messaggio client-server. Gli offset sono da
   *            considerare rispetto al campo data e in particolare
   *            la first_section (non presente esplicitamente)
   *            corrisponde all'inizio di data.string, ossia
   *            offset 0.
   * */
  size_t second_section;
  size_t third_section;
  /*
   * parsed_body: Contiene l'eventuale entity-body. Ne è supportata
   *              la lettura nel modo HTTP/1.0 e con
   *              Transfer-Encoding: chunked
   * */
  struct Dstring parsed_body;
};

// Setup della struct e della memoria dinamica
void CreateHTTPReader(struct HTTPReader *reader);
// Legge la first line e gli headers
int ReadHeaders(struct HTTPReader *reader, int socket);
// Legge l'entity-body, se l'encoding è supportato
int ReadContent(struct HTTPReader *reader, int socket);
// Libera la memoria allocata dinamicamente
void DestroyHTTPReader(struct HTTPReader *reader);
#endif
