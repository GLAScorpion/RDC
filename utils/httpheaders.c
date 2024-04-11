#include "httpheaders.h"

void CreateHTTPHeaders(struct HTTPHeaders *headers) {
  headers->general.Date.string = NULL;
  headers->general.Pragma.string = NULL;
  headers->request.Authorization.string = NULL;
  headers->request.If_Modified_Since.string = NULL;
  headers->request.From.string = NULL;
  headers->request.Referer.string = NULL;
  headers->request.Request_URI.string = NULL;
  headers->request.User_Agent.string = NULL;
  headers->entity.Allow.string = NULL;
  headers->entity.Content_Encoding.string = NULL;
  headers->entity.Content_Length.string = NULL;
  headers->entity.Content_Type.string = NULL;
  headers->entity.Expires.string = NULL;
  headers->entity.Last_Modified.string = NULL;
}

void FieldAdder(struct Dstring *request, struct Dstring *field,
                const char *name) {
  if (field->string != NULL && field->size > 0) {
    ConcatStr(request, name);
    ConcatStr(request, ": ");
    ConcatStr(request, field->string);
    ConcatStr(request, "\r\n");
  }
}

void MakeClientRequestHeaders(struct HTTPHeaders *headers,
                              struct Dstring *request) {
  FieldAdder(request, &headers->general.Date, "Date");
  FieldAdder(request, &headers->general.Pragma, "Pragma");
  FieldAdder(request, &headers->request.Authorization, "Authorization");
  FieldAdder(request, &headers->request.From, "From");
  FieldAdder(request, &headers->request.If_Modified_Since, "If-Modified-Since");
  FieldAdder(request, &headers->request.Referer, "Referer");
  FieldAdder(request, &headers->request.Request_URI, "Request-URI");
  FieldAdder(request, &headers->request.User_Agent, "User-Agent");
  FieldAdder(request, &headers->entity.Allow, "Allow");
  FieldAdder(request, &headers->entity.Content_Encoding, "Content-Encoding");
  FieldAdder(request, &headers->entity.Content_Length, "Content-Length");
  FieldAdder(request, &headers->entity.Content_Type, "Content-Type");
  FieldAdder(request, &headers->entity.Expires, "Expires");
  FieldAdder(request, &headers->entity.Last_Modified, "Last-Modified");
}
