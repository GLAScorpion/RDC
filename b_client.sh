#/bin/sh
gcc -lm ./client/main.c ./client/client.c ./utils/dstring.c ./utils/httpheaders.c ./utils/httpreader.c -o client.out -fsanitize=address -g
./client.out
