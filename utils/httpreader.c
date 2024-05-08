#include "httpreader.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int header_allocator(struct HTTPReader *reader, size_t new_size) {
  if (reader->alloc_size > new_size) {
    return -1;
  }
  if (reader->alloc_size == 0) {
    reader->headers_data = calloc(new_size, sizeof(struct header));
  } else {
    reader->headers_data =
        realloc(reader->headers_data,
                2 << (int)ceil(log2(new_size * sizeof(struct header))));
  }
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
  char buffer[1];
  char parse_cmdline = 1;
  int j = 0;
  int i = 0;
  int k = 0;
  for (; read(socket, buffer, 1); i++) {
    AppendChar(&reader->data, buffer);
    if (reader->data.string[i] == ':' &&
        (reader->headers_data[j].value == -1)) {
      reader->headers_data[j].value = i + 2;
      reader->data.string[i] = 0;
    }
    if (reader->data.string[i] == '\n' && reader->data.string[i - 1] == '\r') {
      reader->data.string[i - 1] = 0;
      if (reader->headers_data[j].name != -1 &&
          reader->data.string[reader->headers_data[j].name] == 0) {
        break;
      }
      header_allocator(reader, j + 2);
      j++;
      reader->headers_data[j].name = i + 1;
    }
  }
  for (k = 0; reader->data.string[k] != ' '; k++) {
  }
  reader->data.string[k] = 0;
  k++;
  reader->second_section = k;
  for (; reader->data.string[k] != ' '; k++) {
  }
  reader->data.string[k] = 0;
  k++;
  reader->third_section = k;

  PopulateHTTPHeaders(&reader->parsed_headers, reader->headers_data,
                      reader->data.string, j);
  return i;
}

int ReadContent(struct HTTPReader *reader, int socket) {
  if (reader->parsed_headers.general.Transfer_Encoding.string == NULL &&
      reader->parsed_headers.entity.Content_Length.string != NULL) {
    DestroyStr(&reader->parsed_body);
    reader->parsed_body.size = reader->parsed_body.alloc_size =
        atoi(reader->parsed_headers.entity.Content_Length.string);
    reader->parsed_body.string = malloc(reader->parsed_body.alloc_size);
    read(socket, reader->parsed_body.string, reader->parsed_body.size);
    return reader->parsed_body.size;
  } else if (reader->parsed_headers.general.Transfer_Encoding.string != NULL &&
             !strcmp(reader->parsed_headers.general.Transfer_Encoding.string,
                     "chunked")) {
    DestroyStr(&reader->parsed_body);
    CreateStr(&reader->parsed_body, "");
    struct Dstring chunk_hex;
    CreateStr(&chunk_hex, "");
    size_t chunk;
    char skip[2];
    char buffer[1];
    char flag = 0;
    char *tmp = NULL;
    for (int i = 0; read(socket, buffer, 1); i++) {
      AppendChar(&chunk_hex, buffer);
      if (chunk_hex.string[chunk_hex.size - 1] == '\r') {
        flag = 1;
      }
      if (chunk_hex.string[chunk_hex.size - 1] == '\n' && flag) {
        chunk_hex.string[chunk_hex.size - 2] = 0;
        flag = 0;
        chunk = strtol(chunk_hex.string, NULL, 16);
        if (chunk == 0) {
          DestroyStr(&chunk_hex);
          ConcatStr(&reader->parsed_body, "\0");
          return reader->parsed_body.size;
        }
        tmp = malloc(chunk + 1);
        for (int k = 0; k < chunk;) {
          k += read(socket, tmp + k, chunk - k);
        }
        read(socket, skip, 2);
        tmp[chunk] = 0;
        ConcatStr(&reader->parsed_body, tmp);
        free(tmp);
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
