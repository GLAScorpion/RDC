#include "client.h"

int main() {
  char localhost[] = "127.0.0.1";
  char google[] = "www.google.com";
  struct Client client;
  create_client(&client);
  int status = connect_client(&client, localhost, 7070);
  printf("Client connection status: %d\n\n", status);
  struct HTTPHeaders headers;
  CreateHTTPHeaders(&headers);
  // CopyStr(&headers.entity.Content_Type, "text/plain; charset=utf-8");
  // CopyStr(&headers.request.Accept_Encoding, "chunked");
  // CopyStr(&headers.general.Connection, "close");
  // CopyStr(&headers.general.Transfer_Encoding, "chunked");
  status = make_request(&client, GET, "/", &headers, "");
  printf("Total bytes read: %d\n", read_all(&client));
  printf("HTTP Version: %s\nStatus Code: %s\nReason: %s\n",
         client.reader.data.string,
         client.reader.data.string + client.reader.second_section,
         client.reader.data.string + client.reader.third_section);
  struct Dstring res_headers;
  CreateStr(&res_headers, "");
  MakeClientRequestHeaders(&client.reader.parsed_headers, &res_headers);
  printf("\n--HEADERS--\n%s--HEADERS END--\n\n", res_headers.string);
  printf("--CONTENT START--\n%s\n--CONTENT END--\n",
         client.reader.parsed_body.string);
  DestroyStr(&res_headers);
  DestroyHTTPHeaders(&headers);
  close_client(&client);
}
