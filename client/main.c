#include "client.h"

int main() {
  struct Client client;

  int status = connect_client(&client, "127.0.0.1", 5000);
  printf("%d", status);
  status = make_request(&client, "/");

  printf("Total bytes read %d\n", read_all(&client));
  printf("%s", client.response);
}
