#include "httpreader.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int header_allocator(struct HTTPReader *reader, size_t new_size) {
  /*
   * Funzione che si occupa di allocare abbastanza memoria
   * per headers_data per contenere tutti gli offset necessari.
   * Se c'è già spazio a sufficienza non alloca altro spazio.
   *
   * */
  if (reader->alloc_size > new_size) {
    return -1;
  }
  if (reader->alloc_size == 0) {
    // In caso di inizializzazione dei dati
    reader->headers_data = calloc(new_size, sizeof(struct header));
  } else {
    // Realloca la memoria sempre in blocchi di byte multipli di 2
    reader->headers_data =
        realloc(reader->headers_data,
                2 << (int)ceil(log2(new_size * sizeof(struct header))));
  }
  // Tutti gli offset vengono impostati al valore "nullo" pari a -1
  for (int i = reader->alloc_size; i < new_size; i++) {
    reader->headers_data[i].value = -1;
    reader->headers_data[i].name = -1;
  }
  reader->alloc_size = new_size;
  return 0;
}

void CreateHTTPReader(struct HTTPReader *reader) {
  reader->alloc_size = 0;
  header_allocator(reader, 2);
  reader->size = 0;
  CreateHTTPHeaders(&reader->parsed_headers);
  CreateStr(&reader->data, "");
  reader->parsed_body.string = NULL;
}

int ReadHeaders(struct HTTPReader *reader, int socket) {
  char buffer; // Buffer singolo byte di lettura
  int i = 0;   // Contatore dei byte letti
  int j = 0;   // Contatore degli headers
  /*
   * Leggo singoli bytes finchè non c'è più nulla
   * da leggere
   *
   * */
  for (; read(socket, &buffer, 1); i++) {
    /*
     * Attacco il char all'ultimo posto di data,
     * e se data termina con 2 zeri il primo di
     * essi viene mantenuto e il char appena
     * inserito inizia una nuova stringa
     *
     * */
    AppendChar(&reader->data, &buffer);
    /*
     * Considero un header con questo formato:
     *      NAME: VALUE
     * Quandi quando incontro un char ':' e il campo
     * value dell'header corrente è nullo (-1) vuol
     * dire che ho appena finito il parse del campo
     * name e il char ':' indica l'inizio del campo
     * value.
     *
     * */
    if (reader->data.string[i] == ':' &&
        (reader->headers_data[j].value == -1)) {
      // setto l'offset del value dell'header a due
      // char dopo così salto il char ':' e lo spazio
      reader->headers_data[j].value = i + 2;
      // Annullo il char del ':' così ho chiuso il
      // campo name e qualsiasi funzione di lettura
      // di stringhe lo considererà come terminatore
      reader->data.string[i] = 0;
    }
    /*
     * Adesso si effettua la verifica della presenza
     * del terminatore del protocollo, ossia CRLF.
     * Esso indica la fine di una riga e corrisponde
     * in particolare alla sequenza di char "\r\n".
     * Quindi in presenza di '\n' verifico che vi
     * sia un '\r' subito prima e procedo.
     *
     * */
    if (reader->data.string[i] == '\n' && reader->data.string[i - 1] == '\r') {
      // Come fatto in precedenza delimito ora il
      // campo value mettendo '\r' a 0
      reader->data.string[i - 1] = 0;
      /*
       * Questo if verifica la presenza di un CRLF
       * subito dopo un CRLF appena precedente
       *
       *       \/               headers_data[j].name punta qui alla fine ciclo
       * \r \n \r \n            1 ciclo
       *  0 /\                  i è /\
       *
       *       \/               headers_data[j].name prima dell'if con il break
       * 0  \n \r \n            3 ciclo
       *        0 /\            i è /\
       *
       * Fondamentalmente questo è l'unico caso in cui è possibile che il campo
       * name punti a uno zero, in qualsiasi altro caso esso punterebbe a un
       * char != da '\r' e quindi in nessun modo verrebbe posto a 0
       *
       * */
      if (reader->headers_data[j].name != -1 &&
          reader->data.string[reader->headers_data[j].name] == 0) {
        break;
      }
      header_allocator(reader, j + 2);
      // Se termino un header con CRLF allora
      // posso incrementare j per un nuovo header
      j++;
      // L'header nuovo inizia dopo '\n' che è
      // il char corrente, quindi punto uno dopo
      reader->headers_data[j].name = i + 1;
    }
  }
  int k; // Contatore per il parse della cmdline
  /*
   * La prima sezione è implicitamente l'offset 0.
   * Quindi per la seconda scorro finchè non c'è
   * uno spazio.
   *
   * */
  for (k = 0; reader->data.string[k] != ' '; k++) {
  }
  // delimito la prima sezione mettendo lo spazio a zero
  reader->data.string[k] = 0;
  // la seconda sezione inizia dopo lo spazio
  k++;
  reader->second_section = k;
  // scorro di nuovo fino al prossimo zero
  for (; reader->data.string[k] != ' '; k++) {
  }
  // delimito la seconda sezione
  reader->data.string[k] = 0;
  // setto la terza sezione
  k++;
  reader->third_section = k;
  /*
   * La terza sezione è già stata delimitata in fase
   * di lettura, non è necessario fare altro.
   *
   * Ora posso popolare la struct HTTPHeaders con i
   * dati di headers_data per un accesso più esplicito
   *
   * */
  PopulateHTTPHeaders(&reader->parsed_headers, reader->headers_data,
                      reader->data.string, j);
  return i;
}

int ReadContent(struct HTTPReader *reader, int socket) {
  if (reader->parsed_headers.general.Transfer_Encoding.string == NULL &&
      reader->parsed_headers.entity.Content_Length.string != NULL) {
    // LETTURA "LEGACY" O FINO A HTTP/1.0
    /*
     * Il messaggio deve presentare l'header Content-Length
     * altrimenti questo tipo di lettura non è possibile e il
     * messaggio è considerabile "Bad".
     *
     * */
    DestroyStr(&reader->parsed_body);
    // Ottengo la dimensione del body dall'header Content-Length
    reader->parsed_body.size = reader->parsed_body.alloc_size =
        atoi(reader->parsed_headers.entity.Content_Length.string);
    // Alloco spazio a sufficienza per il body + 1
    reader->parsed_body.string = malloc(reader->parsed_body.alloc_size + 1);
    // Utilizzo il byte allocato in più per mettere uno zero
    // e assicurarmi di terminare la stringa del body
    reader->parsed_body.string[reader->parsed_body.alloc_size] = 0;
    // Lettura di tutti i byte, tenendo in considerazione
    // eventuali letture parziali
    for (int k = 0; k < reader->parsed_body.size;) {
      k += read(socket, reader->parsed_body.string + k,
                reader->parsed_body.size - k);
    }
    // termino e ritorno il numero di byte letti
    return reader->parsed_body.size;
  } else if (reader->parsed_headers.general.Transfer_Encoding.string != NULL &&
             !strcmp(reader->parsed_headers.general.Transfer_Encoding.string,
                     "chunked")) {
    // LETTURA CON TRANSFER ENCODING CHUNKED (HTTP/1.1+)
    /*
     * Il Transfer-Encoding: chunked utilizza un formato
     * standardizzato che segue questo schema:
     * Chunked-Body = *chunk
     *                 last-chunk
     *                 trailer
     *                 CRLF
     * chunk = chunk-size [ chunk-extension ] CRLF
     *         chunk-data CRLF
     * chunk-size = 1*HEX
     * last-chunk = 1*("0") [ chunk-extension ] CRLF
     *
     * Quindi i chunk sono definiti dalla loro dimensione
     * in byte in esadecimale CRLF e i dati da trasferire.
     * La trasmissione termina con un last-chunk di
     * dimensione 0 e ovviamente niente dati
     *
     * */
    DestroyStr(&reader->parsed_body);
    CreateStr(&reader->parsed_body, "");
    // chunnk_hex è una stringa temporanea che conterrà la
    // dimensione in esadecimale del suo chunk
    struct Dstring chunk_hex;
    CreateStr(&chunk_hex, "");
    size_t chunk; // dimensione del chunk decodificata
    char skip[2]; // "cestino di char"
    char buffer;  // buffer di lettura per singoli char
    // buffer di lettura per i blocchi
    char *tmp = NULL;
    for (int i = 0; read(socket, &buffer, 1); i++) {
      // Appendo i char come durante la lettura degli header
      AppendChar(&chunk_hex, &buffer);
      // Verifico per un CRLF
      if (chunk_hex.string[chunk_hex.size - 1] == '\n' &&
          chunk_hex.string[chunk_hex.size - 2] == '\r') {
        // Azzero il char '\r'
        chunk_hex.string[chunk_hex.size - 2] = 0;
        // Converto la stringa hex, corrispondente alla
        // base 16 in int base 10
        chunk = strtol(chunk_hex.string, NULL, 16);
        /*
         * Come da definizione nel protocollo quando il
         * chunk è == 0 la lettura può terminare
         * */
        if (chunk == 0) {
          // Dealloco chunk_hex e mi assicuro di terminare
          // il body con uno 0. Ritorno i bytes letti.
          DestroyStr(&chunk_hex);
          ConcatStr(&reader->parsed_body, "\0");
          return reader->parsed_body.size;
        }
        /*
         * Alloco lo spazio per il chunk che devo leggere
         * più un byte e azzero l'ultimo byte per evitare
         * comportamenti strani dovuti a strcat
         *
         * */
        tmp = malloc(chunk + 1);
        tmp[chunk] = 0;
        /*
         * Leggo tutto il chunk tenendo conto di letture più
         * piccole del chunk intero
         *
         * */
        for (int k = 0; k < chunk;) {
          k += read(socket, tmp + k, chunk - k);
        }
        // salto il CRLF di terminazione del chunk
        read(socket, skip, 2);
        // Concateno il chunk al resto del body
        ConcatStr(&reader->parsed_body, tmp);
        // Libero il buffer temporaneo
        free(tmp);
        // Rigenero la stringa della dimensione in hex
        DestroyStr(&chunk_hex);
        CreateStr(&chunk_hex, "");
      }
    }
    DestroyStr(&chunk_hex);
  }
  return 0;
}

void DestroyHTTPReader(struct HTTPReader *reader) {
  DestroyStr(&reader->data);
  DestroyStr(&reader->parsed_body);
  DestroyHTTPHeaders(&reader->parsed_headers);
  free(reader->headers_data);
}
