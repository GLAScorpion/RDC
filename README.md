# Simple HTTP Client-Server
Questa repository contiene il progetto per un semplice **Client-Server HTTP**. Il codice è scritto in **C** e segue gli standard **RFC1945** e **RFC2616**, rispettivamente per `HTTP/1.0` e `HTTP/1.1`.
> Per ora è supportato solamente su Linux e non ci sono piani di supporto a Windows o macOS.
## Struttura del progetto
La `root` del progetto è da considerarsi root directory anche per il client e il server.
La directory `utils` contiene librerie comuni ad entrambi i programmi, quali:

- `dstring.h`: Contiene codice relativo a *Dstring*, una struct che funziona come una stringa dinamica
- `httpheaders.h`: Contiene codice realivo a *HTTPHeaders*, una struct che contiene molti degli headers degli standard *HTTP* e offre delle funzioni per generarli e gestirli
- `httpreader.h`: Contiene codice relativo a *HTTPReader*, una struct che contiene i dati necessari per leggere ed effettuare il parse di messaggi *HTTP*. La libreria offre anche le funzioni di parse.

La directory `client` contiene il codice strettamente relativo alla componente *Client*:

- `client.h`: Contiene codice relativo a *Client*, una struct che rappresenta il client, insieme alle varie funzioni che rappresentano le varie operazioni che può eseguire.
- `main.c`: Contiene il *main* di un client basilare che fa una richiesta a un host e ne stampa la risposta a `stdout`

La directory `server` contiene il codice strettamente relativo alla componente *Server*:

- `server.h`: Contiene codice relativo a *Server*, una struct che rappresenta il server, insieme alle funzioni che rappresentano il suo ciclo di esecuzione
- `main.c`: Contiene il *main* del server basilare che si occupa di ricevere e gestire multiple richieste in loop.
## Building
I programmi client e server vengono buildati ed eseguiti rispettivamente dagli script `b_client.sh` e `b_server.sh`. Testati con `gcc (GCC) 13.2.1 20240417`.
Sono stati scritti in ambiente **archlinux** perciò in altri ambienti potrebbero essere necessarie delle modifiche come l'aggiunta dell'opzione `-static-libasan` e sarà necessario accertarsi di soddisfare la dipendeza alla libreria `libasan`.
___
### Consiglio di ordine di lettura
I commenti seguono un ordine di scrittura che è circa simile all'ordine di esecuzione ed è quindi consigliato seguire le varie chiamate a funzione partendo da un main e via via saltando per i vari file.
 L'ordine che di lettura che consiglio personalmente è:

- **main client** -> **client.h** -> **httpreader.h** -> **httpheader.h**
- **main server** -> **server.h** -> **httpreader.h** -> **httpheader.h**

> Quando indico gli header file (`.h`) il consiglio è leggere l'header file e poi il relativo `.c`

## Documentazione utilizzata
- [RFC1945](https://datatracker.ietf.org/doc/html/rfc1945) : Per lo standard `HTTP/1.0`
- [RFC2616](https://datatracker.ietf.org/doc/html/rfc2616) : Per lo standard `HTTP/1.1`
- [die.net](https://linux.die.net/) : Per diverse funzioni relative a *linux*
- [pubs.opengroup.org](https://pubs.opengroup.org/onlinepubs/7908799/xns/) : Per informazioni sulle funzioni e le struct relative alla comunicazione di rete (*inet*)
- [devdocs](https://devdocs.io/) : Per informazioni sul linguaggio *C*

