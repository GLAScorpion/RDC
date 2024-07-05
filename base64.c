#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *encoder(char *str, size_t size);

char base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(int argc, char *argv[]) {
  char *string;
  char *filename;
  char default_file[] = "file.bin";
  // Lettura eventuali nomi di file da cmdline
  if (argc > 1) {
    filename = argv[1];
  } else {
    filename = default_file;
  }
  string = "testa prova";
  int size = strlen(string);
  printf("Encoder call, String to encode: {%s}\n", string);
  char *result = encoder(string, size);
  if (result == NULL) {
    perror("Something went wrong.\nReason");
    return -1;
  }
  printf("The encoded file is:\n%s\n", result);
  free(result); // Libero memoria
}

char *encoder(char *str, size_t size) {
  // apro il file e controllo che esista
  // Numero di blocchi da codificare; da 3 byte
  // ottengo 3 simboli in base64
  // ceil arrotonda per eccesso
  size_t block_num = ceil((double)size / 3);
  // alloco dinamicamente la stringa e mi prendo
  // un byte in più per il terminatore \0
  char *result = malloc((4 * block_num) + 1);
  // Uso un contatore per capire dove sono nella
  // stringa
  size_t res_index = 0;

  // block è la variabile dove andrò a scrivere i
  // byte che leggo e trasformo. Uso un unsigned
  // int così posso usare le operazioni di shift
  uint32_t block = 0;
  // tmp conterrà il valore in base64, estratto
  // da block con shift e maschere
  uint8_t tmp = 0;
  // read è il numero di byte letti a ogni iterazione
  // e servirà per determinare il padding
  // Ciclo per il numero di blocchi
  int read = 0;
  int x;
  for (size_t i = 0; i < block_num; i++) {
    // resetto sempre read perchè mi interessa solo il valore
    // dell'ultima iterazione, le altre saranno sempre read == 3
    // Leggo i 3 byte e li posiziono al contrario all'interno
    // di block per un problema di endianess (credo).
    // In breve se io voglio che i miei byte vengano interpretati
    // come uint così <b1 b2 b3 0> devo invertire il tutto
    // quindi <0 b3 b2 b1>.
    // Tutto ciò perchè io voglio poter shiftare due bit a destra (>>=2)
    // Così il riporto finisce direttamente nel gruppo di bit successivo
    for (x = 3; x > 0 && read < size; x--) {
      memset((char *)(&block) + x, str[read++], 1);
    }
    for (int x = 0; x < 4; x++) {
      // ora il valore che mi interessa si trova a "sinistra" e
      // lo posso leggere con una maschera "11111100000000000000000000000000"
      // e shiftarlo di 26 bit per renderlo del giusto ordine di grandezza.
      // Ad ogni iterazione successiva sposto la maschera a destra di 6 bit
      // e il risultato della AND con la maschera di 26 meno multipli di 6
      tmp = (uint8_t)((block & (0xFC000000 >> (6 * x))) >> (26 - (6 * x)));
      // If rubato a scanta per la conversione base64 >> ASCII ;)
      result[res_index++] = base64[tmp];
    }
    // PADDING
    // Ci possono essere massimo 2 simboli di padding perchè se leggo un
    // byte, mi servono almeno 2 simboli base64 e ogni byte letto in più
    // diminuisce di 1 i padding necessari fino a 3 byte letti e 4 simboli
    // base64 tutti utilizzati.
    // Quindi semplicemente rimpiazzo simboli finchè read non "raggiunge"
    // i 3 byte "letti"

    // resetto il blocco in quanto è importante che la zona di spaziatura non
    // sia inquinata da bit precedenti
    block = 0;
  }
  for (; x > 0; x--) {
    result[--res_index] = 61;
  }
  result[4 * block_num] = 0; // setto il terminatore
  // ritorno
  return result;
}
