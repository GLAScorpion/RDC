#ifndef httpreader_h
#define httpreader_h
#include "dstring.h"
#include "httpheaders.h"

struct HTTPReader {
  struct header *headers_data;
  struct Dstring data;
  size_t alloc_size;
  size_t size;
  struct HTTPHeaders parsed_headers;
  size_t second_section;
  size_t third_section;
  struct Dstring parsed_body;
};

void CreateHTTPReader(struct HTTPReader *reader);

int ReadHeaders(struct HTTPReader *reader, int socket);

int ReadContent(struct HTTPReader *reader, int socket);

void DestroyHTTPReader(struct HTTPReader *reader);
#endif
