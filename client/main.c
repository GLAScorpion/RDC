#include "client.h"
int main() {
  struct Client client;
  create_client(&client);
  int status = connect_client(&client, "127.0.0.1", 5000);
  printf("Client connection status: %d\n", status);
  struct HTTPHeaders headers;
  CreateHTTPHeaders(&headers);
  CopyStr(&headers.entity.Content_Type, "text/plain; charset=utf-8");
  status = make_request(&client, POST, "/post", &headers,
                        "PASTA ALLA CARBONARA WOOOOOOOOOOOO");
  printf("Total bytes read: %d\n", read_all(&client));
  printf("--CONTENT START--\n%s\n--CONTENT END--\n", client.response.string);
  printf("%d %d\n", client.response.size, client.response.alloc_size);
}
