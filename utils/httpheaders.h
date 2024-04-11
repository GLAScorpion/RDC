#ifndef httpheaders_h
#define httpheaders_h
#include "dstring.h"
struct HTTPHeaders {
  struct HGeneral {
    struct Dstring Date;
    struct Dstring Pragma;
  } general;

  struct HRequest {
    struct Dstring Authorization;
    struct Dstring From;
    struct Dstring If_Modified_Since;
    struct Dstring Referer;
    struct Dstring Request_URI;
    struct Dstring User_Agent;
  } request;

  struct HEntity {
    struct Dstring Allow;
    struct Dstring Content_Encoding;
    struct Dstring Content_Length;
    struct Dstring Content_Type;
    struct Dstring Expires;
    struct Dstring Last_Modified;
  } entity;
};

void CreateHTTPHeaders(struct HTTPHeaders *headers);

void MakeClientRequestHeaders(struct HTTPHeaders *headers,
                              struct Dstring *request);
#endif
