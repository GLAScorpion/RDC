#/bin/sh
gcc ./utils/httpheaders.c ./utils/httpreader.c ./utils/dstring.c ./server/server.c ./server/main.c -lm -o server.out
./server.out
