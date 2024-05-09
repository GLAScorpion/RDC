#include "httpheaders.h"
#include <stdio.h>

void CreateHTTPHeaders(struct HTTPHeaders *headers) {
  headers->general.Connection.string = NULL;
  headers->general.Date.string = NULL;
  headers->general.Pragma.string = NULL;
  headers->general.Transfer_Encoding.string = NULL;
  headers->request.Accept_Encoding.string = NULL;
  headers->request.Authorization.string = NULL;
  headers->request.If_Modified_Since.string = NULL;
  headers->request.From.string = NULL;
  headers->request.Referer.string = NULL;
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
  /*
   * Se il campo esiste e non è vuoto, lo formatta
   * con il suo nome, ':', il valore e il CRLF e
   * la concatena all'output
   * */
  if (field->string != NULL && field->size > 0) {
    ConcatStr(request, name);
    ConcatStr(request, ": ");
    ConcatStr(request, field->string);
    ConcatStr(request, "\r\n");
  }
}

void MakeClientRequestHeaders(struct HTTPHeaders *headers,
                              struct Dstring *request) {
  FieldAdder(request, &headers->general.Connection, "Connection");
  FieldAdder(request, &headers->general.Date, "Date");
  FieldAdder(request, &headers->general.Pragma, "Pragma");
  FieldAdder(request, &headers->general.Transfer_Encoding, "Transfer-Encoding");
  FieldAdder(request, &headers->request.Accept_Encoding, "Accept-Encoding");
  FieldAdder(request, &headers->request.Authorization, "Authorization");
  FieldAdder(request, &headers->request.From, "From");
  FieldAdder(request, &headers->request.If_Modified_Since, "If-Modified-Since");
  FieldAdder(request, &headers->request.Referer, "Referer");
  FieldAdder(request, &headers->request.User_Agent, "User-Agent");
  FieldAdder(request, &headers->entity.Allow, "Allow");
  FieldAdder(request, &headers->entity.Content_Encoding, "Content-Encoding");
  FieldAdder(request, &headers->entity.Content_Length, "Content-Length");
  FieldAdder(request, &headers->entity.Content_Type, "Content-Type");
  FieldAdder(request, &headers->entity.Expires, "Expires");
  FieldAdder(request, &headers->entity.Last_Modified, "Last-Modified");
}

void ifFieldPopulate(const char *header_name, const char *header_value,
                     struct Dstring *field, const char *field_name) {
  /*
   * Se header_name == field_name allora il campo
   * è quello corretto, esiste e quindi si può
   * copiare il value nella Dstring corrispondente
   * (field)
   *
   * */
  if (!strcmp(header_name, field_name)) {
    CreateStr(field, header_value);
  }
}

void PopulateHTTPHeaders(struct HTTPHeaders *headers, struct header *header_pos,
                         const char *header_data, size_t size) {
  const char *value = NULL;
  const char *name = NULL;
  CreateHTTPHeaders(headers);
  /*
   * Scorro tutti gli header tranne il primo che
   * è contaminato e non contiene niente di
   * valido
   * */
  for (int i = 1; i < size; i++) {
    value = &header_data[header_pos[i].value];
    name = &header_data[header_pos[i].name];
    // printf("%s: %s\n", name, value);
    ifFieldPopulate(name, value, &headers->general.Connection, "Connection");
    ifFieldPopulate(name, value, &headers->general.Date, "Date");
    ifFieldPopulate(name, value, &headers->general.Pragma, "Pragma");
    ifFieldPopulate(name, value, &headers->general.Transfer_Encoding,
                    "Transfer-Encoding");
    ifFieldPopulate(name, value, &headers->request.Accept_Encoding,
                    "Accept-Encoding");
    ifFieldPopulate(name, value, &headers->request.Authorization,
                    "Authorization");
    ifFieldPopulate(name, value, &headers->request.From, "From");
    ifFieldPopulate(name, value, &headers->request.If_Modified_Since,
                    "If-Modified-Since");
    ifFieldPopulate(name, value, &headers->request.Referer, "Referer");
    ifFieldPopulate(name, value, &headers->request.User_Agent, "User-Agent");
    ifFieldPopulate(name, value, &headers->entity.Allow, "Allow");
    ifFieldPopulate(name, value, &headers->entity.Content_Encoding,
                    "Content-Encoding");
    ifFieldPopulate(name, value, &headers->entity.Content_Length,
                    "Content-Length");
    ifFieldPopulate(name, value, &headers->entity.Content_Type, "Content-Type");
    ifFieldPopulate(name, value, &headers->entity.Expires, "Expires");
    ifFieldPopulate(name, value, &headers->entity.Last_Modified,
                    "Last-Modified");
  }
}

void DestroyHTTPHeaders(struct HTTPHeaders *headers) {
  DestroyStr(&headers->general.Connection);
  DestroyStr(&headers->general.Date);
  DestroyStr(&headers->general.Pragma);
  DestroyStr(&headers->general.Transfer_Encoding);
  DestroyStr(&headers->request.Accept_Encoding);
  DestroyStr(&headers->request.Authorization);
  DestroyStr(&headers->request.If_Modified_Since);
  DestroyStr(&headers->request.From);
  DestroyStr(&headers->request.Referer);
  DestroyStr(&headers->request.User_Agent);
  DestroyStr(&headers->entity.Allow);
  DestroyStr(&headers->entity.Content_Encoding);
  DestroyStr(&headers->entity.Content_Length);
  DestroyStr(&headers->entity.Content_Type);
  DestroyStr(&headers->entity.Expires);
  DestroyStr(&headers->entity.Last_Modified);
}
