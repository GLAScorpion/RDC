#include "client.h"

int main() {
  struct Client client;

  int status = connect_client(&client, "127.0.0.1", 5000);
  printf("Client connection status: %d\n", status);

  status = make_request(&client, POST, "/post",
                        "PASTA ALLA CARBONARA WOOOOOOOOOOOO");
  printf("Total bytes read: %d\n", read_all(&client));
  printf("--CONTENT START--\n%s\n--CONTENT END--\n", client.response);
}
