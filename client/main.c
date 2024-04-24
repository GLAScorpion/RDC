#include "client.h"
#include <netdb.h>

int main() {
  struct hostent *server;
  server = gethostbyname("www.google.com");
  // server = gethostbyname("127.0.0.1");
  struct in_addr **addr_list;
  addr_list = (struct in_addr **)server->h_addr_list;
  struct Client client;
  create_client(&client);
  int status = connect_client(&client, inet_ntoa(*addr_list[0]), 80);
  printf("Client connection status: %d\n", status);
  struct HTTPHeaders headers;
  CreateHTTPHeaders(&headers);
  // CopyStr(&headers.entity.Content_Type, "text/plain; charset=utf-8");
  //  CopyStr(&headers.request.Accept_Encoding, "chunked");
  // CopyStr(&headers.general.Connection, "close");
  // CopyStr(&headers.general.Transfer_Encoding, "chunked");
  status = make_request(&client, HEAD, "/", &headers, "");
  printf("Total bytes read: %d\n", read_all(&client));
  printf("HTTP Version: %s\nStatus Code: %s\nReason: %s\n",
         client.reader.data.string,
         client.reader.data.string + client.reader.second_section,
         client.reader.data.string + client.reader.third_section);
  printf("--CONTENT START--\n%s\n--CONTENT END--\n",
         client.reader.parsed_body.string);
  //   printf("%d %d\n", client.response.size,
  //   client.response.alloc_size);
  /*
  for (size_t i = 0; i < client.response.size; i++) {
    printf("%d ", client.response.string[i]);
    if (client.response.string[i] == '\n') {
      printf("\n");
    }
  }
  */
}
