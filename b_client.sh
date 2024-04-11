#/bin/sh
gcc ./client/main.c ./client/client.c ./utils/dstring.c ./utils/httpheaders.c -o client.out -lm
./client.out
